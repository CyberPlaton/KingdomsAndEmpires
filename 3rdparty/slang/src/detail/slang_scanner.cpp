#include "slang_scanner.hpp"
#include "slang_language_definition.hpp"
#include "slang_error.hpp"

namespace slang
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
			static constexpr auto C_TOKENS_RANGE_START = token_type_true;
			static constexpr auto C_TOKENS_RANGE_END = token_type_return;
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
		inline static stoken make_identifier(detail::cscanning_context& ctx)
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
		inline static stoken make_number(detail::cscanning_context& ctx)
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
		inline static stoken make_string(detail::cscanning_context& ctx)
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
				return make_error(ctx, serrors::errors()[error_type_unterminated_string]);
			}
			else if (is_newline(c))
			{
				return make_error(ctx, serrors::errors()[error_type_multiline_string]);
			}

			ctx.advance();

			return make_token(ctx.cursor().m_line, ctx.cursor().m_text.c_str(), token_type_string);
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline static stoken make_error(detail::cscanning_context& ctx, stringview_t text)
		{
			ctx.result() = compile_result_fail;
			return { text.data(), ctx.cursor().m_line, token_type_error };
		}

	} //- unnamed

	namespace detail
	{
		//------------------------------------------------------------------------------------------------------------------------
		cscanning_context::cscanning_context(const char* code, sconfig cfg /*= {}*/) :
			m_code(code), m_cfg(cfg), m_result(compile_result_ok)
		{
		}

		//------------------------------------------------------------------------------------------------------------------------
		stoken cscanning_context::next_token()
		{
			SLANG_NAMED_CPU_ZONE("ccompiler::next_token");

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
			case slanguage::token_character(token_type_equal):
			{
				//- detect composite equality/inequality
				if (peek(1) == slanguage::token_character(token_type_equal))
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
			case slanguage::token_character(token_type_exclamation):
			{
				//- detect composite equality/inequality
				if (peek(1) == slanguage::token_character(token_type_equal))
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
			case slanguage::token_character(token_type_colon):
			{
				literal = slanguage::C_LITERAL_COLON;
				type = token_type_colon;
				break;
			}
			case slanguage::token_character(token_type_semicolon):
			{
				literal = slanguage::C_LITERAL_SEMICOLON;

				type = token_type_semicolon;
				break;
			}
			case slanguage::token_character(token_type_comma):
			{
				literal = slanguage::C_LITERAL_COMMA;
				type = token_type_comma;
				break;
			}
			case slanguage::token_character(token_type_dot):
			{
				literal = slanguage::C_LITERAL_DOT;
				type = token_type_dot;
				break;
			}
			case slanguage::token_character(token_type_left_bracket):
			{
				literal = slanguage::C_LITERAL_LEFT_BRACKET;
				type = token_type_left_bracket;
				break;
			}
			case slanguage::token_character(token_type_right_bracket):
			{
				literal = slanguage::C_LITERAL_RIGHT_BRACKET;
				type = token_type_right_bracket;
				break;
			}
			case slanguage::token_character(token_type_left_brace):
			{
				literal = slanguage::C_LITERAL_LEFT_BRACE;
				type = token_type_left_brace;
				break;
			}
			case slanguage::token_character(token_type_right_brace):
			{
				literal = slanguage::C_LITERAL_RIGHT_BRACE;
				type = token_type_right_brace;
				break;
			}
			case slanguage::token_character(token_type_left_paren):
			{
				literal = slanguage::C_LITERAL_LEFT_PAREN;
				type = token_type_left_paren;
				break;
			}
			case slanguage::token_character(token_type_right_paren):
			{
				literal = slanguage::C_LITERAL_RIGHT_PAREN;
				type = token_type_right_paren;
				break;
			}
			case slanguage::token_character(token_type_minus):
			{
				literal = slanguage::C_LITERAL_MINUS;
				type = token_type_minus;
				break;
			}
			case slanguage::token_character(token_type_plus):
			{
				literal = slanguage::C_LITERAL_PLUS;
				type = token_type_plus;
				break;
			}
			case slanguage::token_character(token_type_star):
			{
				literal = slanguage::C_LITERAL_STAR;
				type = token_type_star;
				break;
			}
			case slanguage::token_character(token_type_slash):
			{
				literal = slanguage::C_LITERAL_SLASH;
				type = token_type_slash;
				break;
			}
			case slanguage::token_character(token_type_smaller):
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
			case slanguage::token_character(token_type_greater):
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
				literal = serrors::errors()[error_type_unrecognized_literal];
				break;
			}
			}

			//- ignore processed literal
			advance();

			return make_token(cursor().m_line, literal, type);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cscanning_context::process_token(stoken&& token)
		{
			stream().m_tokens.emplace_back(std::move(token));
		}

		//------------------------------------------------------------------------------------------------------------------------
		char cscanning_context::peek(unsigned lookahead /*= 0*/) const
		{
			SLANG_ASSERT(m_cursor.m_current + lookahead < m_code.size() + 1, "Invalid operation. Index out of bound");

			return m_code[m_cursor.m_current + lookahead];
		}

		//------------------------------------------------------------------------------------------------------------------------
		char cscanning_context::advance()
		{
			return m_code[++cursor().m_current];
		}

	} //- detail

	//------------------------------------------------------------------------------------------------------------------------
	cscanner::cscanner(const char* code, detail::cscanning_context::sconfig cfg /*= {}*/) :
		m_ctx(detail::cscanning_context{ code, cfg })
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cscanner::scan()
	{
		SLANG_NAMED_CPU_ZONE("cscanner::scan");

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

		return m_ctx.result() == compile_result_ok;
	}

} //- slang