#include "sm_tiny_shading_language_lexer.hpp"

RTTR_REGISTRATION
{
	using namespace sm::tsl;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::registration::enumeration<token_type>("token_type")
	(
		rttr::value("token_type_none", token_type_none),
		rttr::value("token_type_equality", token_type_equality),
		rttr::value("token_type_smaller_equal", token_type_smaller_equal),
		rttr::value("token_type_greater_equal", token_type_greater_equal),
		rttr::value("token_type_not_equality", token_type_not_equality),
		rttr::value("token_type_equal", token_type_equal),
		rttr::value("token_type_exclamation", token_type_exclamation),
		rttr::value("token_type_colon", token_type_colon),
		rttr::value("token_type_semicolon", token_type_semicolon),
		rttr::value("token_type_comma", token_type_comma),
		rttr::value("token_type_dot", token_type_dot),
		rttr::value("token_type_left_bracket", token_type_left_bracket),
		rttr::value("token_type_right_bracket", token_type_right_bracket),
		rttr::value("token_type_left_brace", token_type_left_brace),
		rttr::value("token_type_right_brace", token_type_right_brace),
		rttr::value("token_type_left_paren", token_type_left_paren),
		rttr::value("token_type_right_paren", token_type_right_paren),
		rttr::value("token_type_minus", token_type_minus),
		rttr::value("token_type_plus", token_type_plus),
		rttr::value("token_type_star", token_type_star),
		rttr::value("token_type_slash", token_type_slash),
		rttr::value("token_type_smaller", token_type_smaller),
		rttr::value("token_type_greater", token_type_greater),
		rttr::value("token_type_directive", token_type_directive),
		rttr::value("token_type_at", token_type_at),

		rttr::value("token_type_void", token_type_void),
		rttr::value("token_type_bool", token_type_bool),
		rttr::value("token_type_int", token_type_int),
		rttr::value("token_type_uint", token_type_uint),
		rttr::value("token_type_float", token_type_float),
		rttr::value("token_type_double", token_type_double),

		rttr::value("token_type_bvec2", token_type_bvec2),
		rttr::value("token_type_bvec3", token_type_bvec3),
		rttr::value("token_type_bvec4", token_type_bvec4),
		rttr::value("token_type_ivec2", token_type_ivec2),
		rttr::value("token_type_ivec3", token_type_ivec3),
		rttr::value("token_type_ivec4", token_type_ivec4),
		rttr::value("token_type_uvec2", token_type_uvec2),
		rttr::value("token_type_uvec3", token_type_uvec3),
		rttr::value("token_type_uvec4", token_type_uvec4),
		rttr::value("token_type_vec2", token_type_vec2),
		rttr::value("token_type_vec3", token_type_vec3),
		rttr::value("token_type_vec4", token_type_vec4),
		rttr::value("token_type_dvec2", token_type_dvec2),
		rttr::value("token_type_dvec3", token_type_dvec3),
		rttr::value("token_type_dvec4", token_type_dvec4),

		rttr::value("token_type_mat2", token_type_mat2),
		rttr::value("token_type_mat3", token_type_mat3),
		rttr::value("token_type_mat4", token_type_mat4),

		rttr::value("token_type_sampler1D", token_type_sampler1D),
		rttr::value("token_type_sampler2D", token_type_sampler2D),

		rttr::value("token_type_attribute", token_type_attribute),
		rttr::value("token_type_const", token_type_const),
		rttr::value("token_type_uniform", token_type_uniform),
		rttr::value("token_type_varying", token_type_varying),
		rttr::value("token_type_break", token_type_break),
		rttr::value("token_type_continue", token_type_continue),
		rttr::value("token_type_discard", token_type_discard),
		rttr::value("token_type_for", token_type_for),
		rttr::value("token_type_while", token_type_while),
		rttr::value("token_type_do", token_type_do),
		rttr::value("token_type_if", token_type_if),
		rttr::value("token_type_else", token_type_else),
		rttr::value("token_type_switch", token_type_switch),
		rttr::value("token_type_case", token_type_case),
		rttr::value("token_type_default", token_type_default),
		rttr::value("token_type_in", token_type_in),
		rttr::value("token_type_out", token_type_out),
		rttr::value("token_type_inout", token_type_inout),
		rttr::value("token_type_layout", token_type_layout),
		rttr::value("token_type_invariant", token_type_invariant),
		rttr::value("token_type_precision", token_type_precision),
		rttr::value("token_type_highp", token_type_highp),
		rttr::value("token_type_mediump", token_type_mediump),
		rttr::value("token_type_lowp", token_type_lowp),
		rttr::value("token_type_return", token_type_return),

		rttr::value("token_type_common", token_type_common),
		rttr::value("token_type_partition", token_type_partition),
		rttr::value("token_type_active", token_type_active),
		rttr::value("token_type_asm", token_type_asm),
		rttr::value("token_type_class", token_type_class),
		rttr::value("token_type_union", token_type_union),
		rttr::value("token_type_enum", token_type_enum),
		rttr::value("token_type_typedef", token_type_typedef),
		rttr::value("token_type_template", token_type_template),
		rttr::value("token_type_this", token_type_this),
		rttr::value("token_type_packed", token_type_packed),
		rttr::value("token_type_goto", token_type_goto),
		rttr::value("token_type_inline", token_type_inline),
		rttr::value("token_type_noinline", token_type_noinline),
		rttr::value("token_type_volatile", token_type_volatile),
		rttr::value("token_type_public", token_type_public),
		rttr::value("token_type_static", token_type_static),
		rttr::value("token_type_extern", token_type_extern),
		rttr::value("token_type_external", token_type_external),
		rttr::value("token_type_interface", token_type_interface),
		rttr::value("token_type_long", token_type_long),
		rttr::value("token_type_short", token_type_short),
		rttr::value("token_type_half", token_type_half),
		rttr::value("token_type_fixed", token_type_fixed),
		rttr::value("token_type_unsigned", token_type_unsigned),
		rttr::value("token_type_superp", token_type_superp),
		rttr::value("token_type_input", token_type_input),
		rttr::value("token_type_output", token_type_output),
		rttr::value("token_type_hvec2", token_type_hvec2),
		rttr::value("token_type_hvec3", token_type_hvec3),
		rttr::value("token_type_hvec4", token_type_hvec4),
		rttr::value("token_type_fvec2", token_type_fvec2),
		rttr::value("token_type_fvec3", token_type_fvec3),
		rttr::value("token_type_fvec4", token_type_fvec4),
		rttr::value("token_type_filter", token_type_filter),
		rttr::value("token_type_true", token_type_true),
		rttr::value("token_type_false", token_type_false),
		rttr::value("token_type_VERTEX", token_type_VERTEX),
		rttr::value("token_type_PIXEL", token_type_PIXEL),
		rttr::value("token_type_fn", token_type_fn),
		rttr::value("token_type_struct", token_type_struct),

		rttr::value("token_type_eof", token_type_eof),
		rttr::value("token_type_error", token_type_error),
		rttr::value("token_type_identifier", token_type_identifier),
		rttr::value("token_type_number", token_type_number),
		rttr::value("token_type_string", token_type_string),

		rttr::value("token_type_count", token_type_count)
	);
}

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
				return c == '-';
			}

			//------------------------------------------------------------------------------------------------------------------------
			inline static std::pair<bool, token_type> is_keyword(stringview_t text)
			{
				static constexpr auto C_TOKENS_RANGE_START = token_type_attribute;
				static constexpr auto C_TOKENS_RANGE_END = token_type_struct;
				static const auto compare = [](stringview_t a, stringview_t b) -> const bool
					{
						return a.size() == b.size() && std::memcmp(a.data(), b.data(), a.size()) == 0;
					};

				for (auto i = (byte_t)C_TOKENS_RANGE_START; i <= (byte_t)C_TOKENS_RANGE_END; ++i)
				{
					if (compare(slanguage::tokens()[i], text))
					{
						return { true, (token_type)i };
					}
				}
				return { false, token_type_none };
			}

			//------------------------------------------------------------------------------------------------------------------------
			inline static std::pair<bool, token_type> is_data_type(stringview_t text)
			{
				static constexpr auto C_TOKENS_RANGE_START = token_type_void;
				static constexpr auto C_TOKENS_RANGE_END = token_type_sampler2D;
				static const auto compare = [](stringview_t a, stringview_t b) -> const bool
					{
						return a.size() == b.size() && std::memcmp(a.data(), b.data(), a.size()) == 0;
					};

				for (auto i = (byte_t)C_TOKENS_RANGE_START; i < (byte_t)C_TOKENS_RANGE_END; ++i)
				{
					if (compare(slanguage::tokens()[i], text))
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

				while (is_identifier(c) || is_digit(c))
				{
					append_one_char(ctx.cursor().m_text, c);

					c = ctx.advance();
				}

				stringview_t text = ctx.cursor().m_text.c_str();

				//- identify keywords and data types or return as normal identifier
				if (const auto [result, type] = is_keyword(text); result)
				{
					return make_token(ctx.cursor().m_line, text, type);
				}
				else if (const auto [result, type] = is_data_type(text); result)
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

				//- Eat up the starting quote and move to actual string text
				ctx.advance();

				auto c = ctx.peek();

				string_t s; append_one_char(s, c);

				//- Iterate until we encounter the second quote, or until the end of file or new line, the latter
				//- we do not support and will report an error!
				while (!string_utils::compare(s, slanguage::C_LITERAL_QUOTE.data()) && !is_eof(c) && !is_newline(c))
				{
					append_one_char(ctx.cursor().m_text, c);

					c = ctx.advance();

					s.clear();

					append_one_char(s, c);
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
					if (peek(1) == character(token_type_minus))
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
				}

				string_t literal;
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
				case character(token_type_directive):
				{
					literal = slanguage::C_LITERAL_DIRECTIVE;
					type = token_type_directive;
					break;
				}
				case character(token_type_at):
				{
					literal = slanguage::C_LITERAL_AT;
					type = token_type_at;
					break;
				}
				case character(token_type_quote):
				{
					return make_string(*this);
				}
				default:
				{
					//- report error
					literal = fmt::format("Unrecognized literal '{}'!", c);
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
		string_t slanguage::token_type_text(token_type type)
		{
			return algorithm::enum_to_string(type);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void slanguage::validate_token_mapping()
		{
			CORE_ASSERT(token_type_count == slanguage::C_TOKENS.size(), "Invalid operation. C_TOKENS size mismatch with token_type enum count");

			static const auto compare = [](stringview_t a, stringview_t b) -> const bool
				{
					return a.size() == b.size() && std::memcmp(a.data(), b.data(), a.size()) == 0;
				};

			log_debug("Validating tiny shader language token mapping");

			//- Ignore the starting literal tokens as they dont change and last helper token
			for (auto i = (byte_t)(((int)token_type_eof) + 1); i < (byte_t)token_type_count; ++i)
			{
				const auto& token = slanguage::tokens()[i];
				const auto token_text = fmt::format("token_type_{}", token);
				const auto enum_text = algorithm::enum_to_string((token_type)i);

				if (!compare(token_text, enum_text))
				{
					log_critical(fmt::format("Language token mapping mismatch for  '{}':'{}'", enum_text, token_text));
					CORE_ASSERT(false, "Invalid operation. C_TOKENS mismatch with token_type enum");
				}
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		ctiny_shader_language_lexer::ctiny_shader_language_lexer(const char* source, lexing::sconfig cfg /*= {}*/) :
			m_ctx(source, cfg)
		{
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool ctiny_shader_language_lexer::scan()
		{
#if DEBUG
			slanguage::validate_token_mapping();
#endif
			for (;;)
			{
				auto token = m_ctx.next_token();
				auto type = token.m_type;

				log_debug(fmt::format("Processed token: [{}][{}][{}]", slanguage::token_type_text(token.m_type),
					token.m_text,
					token.m_line));

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