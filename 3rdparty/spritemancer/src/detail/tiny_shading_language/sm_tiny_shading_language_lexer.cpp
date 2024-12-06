#include "sm_tiny_shading_language_lexer.hpp"

namespace sm
{
	namespace tsl
	{
		namespace
		{
			//------------------------------------------------------------------------------------------------------------------------
			inline static void append_one_char(string_t& string, char c)
			{
				const char* start = &c;
				const char* end = start + sizeof(char);

				string.append(start, end);
			}

			//------------------------------------------------------------------------------------------------------------------------
			inline static bool is_identifier(char c)
			{
				return ((c >= 'a' && c <= 'z') ||
					(c >= 'A' && c <= 'Z') ||
					c == '_');
			}

			//------------------------------------------------------------------------------------------------------------------------
			inline static bool is_eof(char c)
			{
				return c == '\0';
			}

			//------------------------------------------------------------------------------------------------------------------------
			inline static bool is_digit(char c)
			{
				return c >= '0' && c <= '9';
			}

			//------------------------------------------------------------------------------------------------------------------------
			inline static bool is_whitespace(char c)
			{
				return (c == '\n' || c == '\t' ||
					c == '\f' || c == '\r' ||
					c == ' ');
			}

			//------------------------------------------------------------------------------------------------------------------------
			inline static bool is_newline(char c)
			{
				return c == '\n';
			}

			//------------------------------------------------------------------------------------------------------------------------
			inline static bool is_comment(char c)
			{
				return c == '#';
			}

			//------------------------------------------------------------------------------------------------------------------------
			inline static std::pair<bool, token_type> is_keyword(stringview_t text)
			{
				static constexpr auto C_TOKENS_RANGE_START = token_type_attribute;
				static constexpr auto C_TOKENS_RANGE_END = token_type_filter;
				static const auto compare = [](stringview_t a, stringview_t b) -> const bool
					{
						return a.size() == b.size() && std::memcmp(a.data(), b.data(), a.size()) == 0;
					};

				const auto& tokens = slanguage::tokens();

				for (auto i = (byte_t)C_TOKENS_RANGE_START; i < (byte_t)C_TOKENS_RANGE_END; ++i)
				{
					if (compare(tokens[i], text))
					{
						return { true, (token_type)i };
					}
				}
				return { false, token_type_none };
			}

			//------------------------------------------------------------------------------------------------------------------------
			inline static stoken make_token(unsigned line, stringview_t text, token_type type)
			{
				return { text.data(), line, type };
			}

			//------------------------------------------------------------------------------------------------------------------------
			inline static stoken make_error(detail::clexing_context& ctx, stringview_t text)
			{
				ctx.result() = false;
				return { text.data(), ctx.cursor().m_line, token_type_error };
			}

			//------------------------------------------------------------------------------------------------------------------------
			inline static stoken make_identifier(detail::clexing_context& ctx)
			{
				ctx.cursor().m_text.clear();

				auto c = ctx.peek();

				while (is_identifier(c))
				{
					append_one_char(ctx.cursor().m_text, c);

					c = ctx.advance();
				}

				stringview_t text = ctx.cursor().m_text.c_str();

				//- identify keywords or return as normal identifier
				if (const auto [result, type] = is_keyword(text); result)
				{
					return make_token(ctx.cursor().m_line, text, type);
				}
				return make_token(ctx.cursor().m_line, text, token_type_identifier);
			}

			//------------------------------------------------------------------------------------------------------------------------
			inline static stoken make_number(detail::clexing_context& ctx)
			{
				ctx.cursor().m_text.clear();

				auto c = ctx.peek();

				while (is_digit(c))
				{
					append_one_char(ctx.cursor().m_text, c);

					c = ctx.advance();
				}

				//- detect floating point numbers
				if (c == '.')
				{
					append_one_char(ctx.cursor().m_text, '.');

					//- get the rest of the number
					c = ctx.advance();

					while (is_digit(c))
					{
						append_one_char(ctx.cursor().m_text, c);

						c = ctx.advance();
					}
				}
				return make_token(ctx.cursor().m_line, ctx.cursor().m_text.c_str(), token_type_number);
			}

			//------------------------------------------------------------------------------------------------------------------------
			inline static stoken make_string(detail::clexing_context& ctx)
			{
				ctx.cursor().m_text.clear();

				auto c = ctx.peek();

				while (c != '"' && !is_eof(c) && !is_newline(c))
				{
					append_one_char(ctx.cursor().m_text, c);

					c = ctx.advance();
				}

				//- strings must be terminated and only be on one line
				if (is_eof(c))
				{
					return make_error(ctx, "Unterminated string!");
				}
				else if (is_newline(c))
				{
					return make_error(ctx, "Multiline string!");
				}

				ctx.advance();

				return make_token(ctx.cursor().m_line, ctx.cursor().m_text.c_str(), token_type_string);
			}

			//------------------------------------------------------------------------------------------------------------------------
			inline static constexpr char character(token_type type)
			{
				return slanguage::token_character(type);
			}

		} //- unnamed

		namespace detail
		{
			//------------------------------------------------------------------------------------------------------------------------
			clexing_context::clexing_context(const char* source, lexing::sconfig cfg /*= {}*/) :
				m_code(source), m_cfg(cfg), m_result(true)
			{
			}

			//------------------------------------------------------------------------------------------------------------------------
			void clexing_context::process_token(stoken&& token)
			{
				stream().emplace_back(std::move(token));
			}

			//------------------------------------------------------------------------------------------------------------------------
			char clexing_context::peek(unsigned lookahead /*= 0*/) const
			{
				return m_code[m_cursor.m_current + lookahead];
			}

			//------------------------------------------------------------------------------------------------------------------------
			char clexing_context::advance()
			{
				return m_code[++cursor().m_current];
			}

			//------------------------------------------------------------------------------------------------------------------------
			stoken clexing_context::next_token()
			{
				cursor().m_text.clear();

				auto c = peek();

				//- ignore whitespaces
				while (is_whitespace(c))
				{
					if (is_newline(c))
					{
						++cursor().m_line;
					}
					c = advance();
				}

				//- end of file
				if (is_eof(c))
				{
					return make_token(cursor().m_line, "\0", token_type_eof);
				}

				//- numbers
				if (is_digit(c))
				{
					return make_number(*this);
				}

				//- identifiers
				if (is_identifier(c))
				{
					return make_identifier(*this);
				}

				//- comments
				if (is_comment(c))
				{
					//- ignore until end of current line
					c = advance();

					while (!is_newline(c))
					{
						c = advance();
					}

					++cursor().m_line;

					//- begin scanning from start
					return next_token();
				}

				stringview_t literal;
				token_type type = token_type_none;

				//- basic literals
				switch (c)
				{
				case character(token_type_equal):
				{
					//- detect composite equality/inequality
					if (peek(1) == character(token_type_equal))
					{
						//- ==
						literal = slanguage::C_LITERAL_EQUAL_EQUAL;
						type = token_type_equality;
					}
					else
					{
						//- =
						literal = slanguage::C_LITERAL_EQUAL;
						type = token_type_equal;
					}
					break;
				}
				case character(token_type_exclamation):
				{
					//- detect composite equality/inequality
					if (peek(1) == character(token_type_equal))
					{
						//- !=
						literal = slanguage::C_LITERAL_NOT_EQUAL;
						type = token_type_not_equality;
					}
					else
					{
						//- !
						literal = slanguage::C_LITERAL_EXCLAMATION;
						type = token_type_exclamation;
					}
					break;
				}
				case character(token_type_colon):
				{
					literal = slanguage::C_LITERAL_COLON;
					type = token_type_colon;
					break;
				}
				case character(token_type_semicolon):
				{
					literal = slanguage::C_LITERAL_SEMICOLON;

					type = token_type_semicolon;
					break;
				}
				case character(token_type_comma):
				{
					literal = slanguage::C_LITERAL_COMMA;
					type = token_type_comma;
					break;
				}
				case character(token_type_dot):
				{
					literal = slanguage::C_LITERAL_DOT;
					type = token_type_dot;
					break;
				}
				case character(token_type_left_bracket):
				{
					literal = slanguage::C_LITERAL_LEFT_BRACKET;
					type = token_type_left_bracket;
					break;
				}
				case character(token_type_right_bracket):
				{
					literal = slanguage::C_LITERAL_RIGHT_BRACKET;
					type = token_type_right_bracket;
					break;
				}
				case character(token_type_left_brace):
				{
					literal = slanguage::C_LITERAL_LEFT_BRACE;
					type = token_type_left_brace;
					break;
				}
				case character(token_type_right_brace):
				{
					literal = slanguage::C_LITERAL_RIGHT_BRACE;
					type = token_type_right_brace;
					break;
				}
				case character(token_type_left_paren):
				{
					literal = slanguage::C_LITERAL_LEFT_PAREN;
					type = token_type_left_paren;
					break;
				}
				case character(token_type_right_paren):
				{
					literal = slanguage::C_LITERAL_RIGHT_PAREN;
					type = token_type_right_paren;
					break;
				}
				case character(token_type_minus):
				{
					literal = slanguage::C_LITERAL_MINUS;
					type = token_type_minus;
					break;
				}
				case character(token_type_plus):
				{
					literal = slanguage::C_LITERAL_PLUS;
					type = token_type_plus;
					break;
				}
				case character(token_type_star):
				{
					literal = slanguage::C_LITERAL_STAR;
					type = token_type_star;
					break;
				}
				case character(token_type_slash):
				{
					literal = slanguage::C_LITERAL_SLASH;
					type = token_type_slash;
					break;
				}
				case character(token_type_smaller):
				{
					//- detect composite equality/inequality
					if (peek(1) == '=')
					{
						//- <=
						literal = slanguage::C_LITERAL_SMALLER_EQUAL;
						type = token_type_smaller_equal;
					}
					else
					{
						//- <
						literal = slanguage::C_LITERAL_SMALLER;
						type = token_type_smaller;
					}
					break;
				}
				case character(token_type_greater):
				{
					//- detect composite equality/inequality
					if (peek(1) == '=')
					{
						//- >=
						literal = slanguage::C_LITERAL_GREATER_EQUAL;
						type = token_type_greater_equal;
					}
					else
					{
						//- >
						literal = slanguage::C_LITERAL_GREATER;
						type = token_type_greater;
					}
					break;
				}
				default:
				{
					//- report error
					literal = "Unrecognized literal!";
					break;
				}
				}

				//- ignore processed literal
				advance();

				return make_token(cursor().m_line, literal, type);
			}

		} //- detail

		//------------------------------------------------------------------------------------------------------------------------
		const array_t<stringview_t, token_type_count>& slanguage::tokens()
		{
			return C_TOKENS;
		}

		//------------------------------------------------------------------------------------------------------------------------
		ctiny_shader_language_lexer::ctiny_shader_language_lexer(const char* source, lexing::sconfig cfg /*= {}*/) :
			m_ctx(source, cfg)
		{
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool ctiny_shader_language_lexer::scan()
		{
			for (;;)
			{
				auto token = m_ctx.next_token();
				auto type = token.m_type;

				m_ctx.process_token(std::move(token));

				if (type == token_type_eof || type == token_type_error)
				{
					break;
				}
			}

			return m_ctx.result();
		}

	} //- tsl

} //- sm