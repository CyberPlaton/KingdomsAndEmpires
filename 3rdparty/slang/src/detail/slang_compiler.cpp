#include "slang_compiler.hpp"
#include "slang_debug.hpp"
#include <string.h> //- std::stoi and the like

namespace slang
{
	namespace detail
	{
		namespace
		{
			//- TODO: consider moving error messages to a specific file in a namespace, i.e. ::detail::errors{}
			constexpr stringview_t C_ERROR_UNRECOGNIZED_LITERAL = "An unrecognized literal was encountered";
			constexpr stringview_t C_ERROR_UNTERMINATED_STRING = "Unterminated string. String declarations must end with '\"'";
			constexpr stringview_t C_ERROR_MULTILINE_STRING = "Multiline strings are not supported. Make sure the string ends with '\"' on the same line it was declared on";
			constexpr stringview_t C_ERROR_EOF_EXPECTED = "Expected 'end of file' expression";

			//- TODO: currently we have to duplicate tokens for compiler and for debug,
			//- consider moving them to one place, i.e. a place of constants
			constexpr stringview_t C_TOKEN_TRUE		= "true";
			constexpr stringview_t C_TOKEN_FALSE	= "false";
			constexpr stringview_t C_TOKEN_NULL		= "null";
			constexpr stringview_t C_TOKEN_CLASS	= "class";
			constexpr stringview_t C_TOKEN_DEF		= "def";
			constexpr stringview_t C_TOKEN_RETURN	= "return";
			constexpr stringview_t C_TOKEN_VAR		= "var";
			constexpr stringview_t C_TOKEN_THIS		= "this";

			constexpr stringview_t C_LITERAL_EQUAL_EQUAL	= "==";
			constexpr stringview_t C_LITERAL_SMALLER_EQUAL	= "<=";
			constexpr stringview_t C_LITERAL_GREATER_EQUAL	= ">=";
			constexpr stringview_t C_LITERAL_NOT_EQUAL		= "!=";
			constexpr stringview_t C_LITERAL_EQUAL			= "=";
			constexpr stringview_t C_LITERAL_EXCLAMATION	= "!";
			constexpr stringview_t C_LITERAL_COLON			= ":";
			constexpr stringview_t C_LITERAL_SEMICOLON		= ";";
			constexpr stringview_t C_LITERAL_COMMA			= ",";
			constexpr stringview_t C_LITERAL_DOT			= ".";
			constexpr stringview_t C_LITERAL_LEFT_BRACKET	= "[";
			constexpr stringview_t C_LITERAL_RIGHT_BRACKET	= "]";
			constexpr stringview_t C_LITERAL_LEFT_BRACE		= "{";
			constexpr stringview_t C_LITERAL_RIGHT_BRACE	= "}";
			constexpr stringview_t C_LITERAL_LEFT_PAREN		= "(";
			constexpr stringview_t C_LITERAL_RIGHT_PAREN	= ")";
			constexpr stringview_t C_LITERAL_MINUS			= "-";
			constexpr stringview_t C_LITERAL_PLUS			= "+";
			constexpr stringview_t C_LITERAL_STAR			= "*";
			constexpr stringview_t C_LITERAL_SLASH			= "/";
			constexpr stringview_t C_LITERAL_SMALLER		= "<";
			constexpr stringview_t C_LITERAL_GREATER		= ">";

			constexpr token_type C_TOKEN_KEYWORD_RANGE_START = token_type_true;
			constexpr token_type C_TOKEN_KEYWORD_RANGE_END = token_type_return;

			//------------------------------------------------------------------------------------------------------------------------
			bool check_keyword(stringview_t text, token_type type)
			{
				const auto compare = [](stringview_t a, stringview_t b) -> const bool
					{
						return a.size() == b.size() && std::memcmp(a.data(), b.data(), a.size()) == 0;
					};

				switch (type)
				{
				case token_type_class: { return compare(text, C_TOKEN_CLASS); }
				case token_type_false: { return compare(text, C_TOKEN_FALSE); }
				case token_type_true: { return compare(text, C_TOKEN_TRUE); }
				case token_type_null: { return compare(text, C_TOKEN_NULL); }
				case token_type_def: { return compare(text, C_TOKEN_DEF); }
				case token_type_return: { return compare(text, C_TOKEN_RETURN); }
				case token_type_var: { return compare(text, C_TOKEN_VAR); }
				case token_type_this: { return compare(text, C_TOKEN_THIS); }
				default:
					break;
				}
				return false;
			}

			//------------------------------------------------------------------------------------------------------------------------
			std::pair<bool, token_type> check_keywords(stringview_t text)
			{
				for (auto i = (uint8_t)C_TOKEN_KEYWORD_RANGE_START; i < (uint8_t)C_TOKEN_KEYWORD_RANGE_END; ++i)
				{
					if (check_keyword(text, (token_type)i))
					{
						return {true, (token_type)i};
					}
				}
				return {false, token_type_none};
			}

			//------------------------------------------------------------------------------------------------------------------------
			static void append_one_char(string_t& string, char c)
			{
				const char* start = &c;
				const char* end = start + sizeof(char);

				string.append(start, end);
			}

		}; //- unnamed

		//------------------------------------------------------------------------------------------------------------------------
		bool ccompiler::sscanner::is_identifier(char c) const
		{
			return ((c >= 'a' && c <= 'z') ||
					(c >= 'A' && c <= 'Z') ||
					c == '_');
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool ccompiler::sscanner::is_eof(char c) const
		{
			return c == '\0';
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool ccompiler::sscanner::is_digit(char c) const
		{
			return c >= '0' && c <= '9';
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool ccompiler::sscanner::is_whitespace(char c) const
		{
			return (c == '\n' || c == '\t' ||
					c == '\f' || c == '\r' ||
					c == ' ');
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool ccompiler::sscanner::is_newline(char c) const
		{
			return c == '\n';
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool ccompiler::sscanner::is_comment(char c) const
		{
			return c == '#';
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool ccompiler::sscanner::is_keyword(stringview_t text, token_type type) const
		{
			return check_keyword(text, type);
		}

		//------------------------------------------------------------------------------------------------------------------------
		stoken ccompiler::sscanner::make_identifier()
		{
			m_cursor.m_text.clear();
			
			auto c = peek();
			
			while(is_identifier(c))
			{
				append_one_char(m_cursor.m_text, c);

				c = advance();
			}

			stringview_t text = m_cursor.m_text.c_str();

			//- identify keywords or return as normal identifier
			if (const auto [result, type] = check_keywords(text); result)
			{
				return make_token(m_cursor.m_line, text, type);
			}
			return make_token(m_cursor.m_line, text, token_type_identifier);
		}

		//------------------------------------------------------------------------------------------------------------------------
		slang::detail::stoken ccompiler::sscanner::make_number()
		{
			m_cursor.m_text.clear();

			auto c = peek();

			while (is_digit(c))
			{
				append_one_char(m_cursor.m_text, c);

				c = advance();
			}

			//- detect floating point numbers
			if (c == '.')
			{
				append_one_char(m_cursor.m_text, '.');

				//- get the rest of the number
				c = advance();

				while (is_digit(c))
				{
					append_one_char(m_cursor.m_text, c);

					c = advance();
				}
			}
			return make_token(m_cursor.m_line, m_cursor.m_text.c_str(), token_type_number);
		}

		//------------------------------------------------------------------------------------------------------------------------
		slang::detail::stoken ccompiler::sscanner::make_string()
		{
			m_cursor.m_text.clear();

			auto c = peek();

			while (c != '"' && !is_eof(c) && !is_newline(c))
			{
				append_one_char(m_cursor.m_text, c);

				c = advance();
			}

			//- strings must be terminated and only be on one line
			if (is_eof(c))
			{
				return make_error(C_ERROR_UNTERMINATED_STRING);
			}
			else if (is_newline(c))
			{
				return make_error(C_ERROR_MULTILINE_STRING);
			}

			advance();

			return make_token(m_cursor.m_line, m_cursor.m_text.c_str(), token_type_string);
		}

		//------------------------------------------------------------------------------------------------------------------------
		slang::detail::stoken ccompiler::sscanner::make_error(stringview_t text)
		{
			m_result = compile_result_fail;
			return {m_cursor.m_line, text.data(), token_type_error};
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccompiler::reset()
		{
			m_compiler.m_chunk.m_code.clear();
			m_compiler.m_chunk.m_constants.clear();
			m_scanner.m_cursor.m_current = 0;
			m_scanner.m_cursor.m_text.clear();
			m_scanner.m_cursor.m_line = 0;
			m_scanner.m_token_stream.m_stream.clear();
			m_scanner.m_result = compile_result_ok;
		}

		//------------------------------------------------------------------------------------------------------------------------
		detail::stoken ccompiler::sscanner::next_token()
		{
			ZoneScopedN("ccompiler::next_token");

			m_cursor.m_text.clear();

			auto c = peek();

			//- ignore whitespaces
			while (is_whitespace(c))
			{
				if (is_newline(c))
				{
					++m_cursor.m_line;
				}
				c = advance();
			}

			//- end of file
			if (is_eof(c))
			{
				return make_token(m_cursor.m_line, "\0", token_type_eof);
			}

			//- numbers
			if (is_digit(c))
			{
				return make_number();
			}

			//- identifiers
			if (is_identifier(c))
			{
				return make_identifier();
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

				++m_cursor.m_line;

				//- begin scanning from start
				return next_token();
			}

			stringview_t literal;
			token_type type = token_type_none;

			//- basic literals
			switch (c)
			{
			case '=':
			{
				//- detect composite equality/inequality
				if (peek(1) == '=')
				{
					//- ==
					literal = C_LITERAL_EQUAL_EQUAL;
					type = token_type_equality;
				}
				else
				{
					//- =
					literal = C_LITERAL_EQUAL;
					type = token_type_equal;
				}
				break;
			}
			case '!':
			{
				//- detect composite equality/inequality
				if (peek(1) == '=')
				{
					//- !=
					literal = C_LITERAL_NOT_EQUAL;
					type = token_type_not_equality;
				}
				else
				{
					//- !
					literal = C_LITERAL_EXCLAMATION;
					type = token_type_exclamation;
				}
				break;
			}
			case ':':
			{
				literal = C_LITERAL_COLON;
				type = token_type_colon;
				break;
			}
			case ';':
			{
				literal = C_LITERAL_SEMICOLON;

				type = token_type_semicolon;
				break;
			}
			case ',':
			{
				literal = C_LITERAL_COMMA;
				type = token_type_comma;
				break;
			}
			case '.':
			{
				literal = C_LITERAL_DOT;
				type = token_type_dot;
				break;
			}
			case '[':
			{
				literal = C_LITERAL_LEFT_BRACKET;
				type = token_type_left_bracket;
				break;
			}
			case ']':
			{
				literal = C_LITERAL_RIGHT_BRACKET;
				type = token_type_right_bracket;
				break;
			}
			case '{':
			{
				literal = C_LITERAL_LEFT_BRACE;
				type = token_type_left_brace;
				break;
			}
			case '}':
			{
				literal = C_LITERAL_RIGHT_BRACE;
				type = token_type_right_brace;
				break;
			}
			case '(':
			{
				literal = C_LITERAL_LEFT_PAREN;
				type = token_type_left_paren;
				break;
			}
			case ')':
			{
				literal = C_LITERAL_RIGHT_PAREN;
				type = token_type_right_paren;
				break;
			}
			case '-':
			{
				literal = C_LITERAL_MINUS;
				type = token_type_minus;
				break;
			}
			case '+':
			{
				literal = C_LITERAL_PLUS;
				type = token_type_plus;
				break;
			}
			case '*':
			{
				literal = C_LITERAL_STAR;
				type = token_type_star;
				break;
			}
			case '/':
			{
				literal = C_LITERAL_SLASH;
				type = token_type_slash;
				break;
			}
			case '<':
			{
				//- detect composite equality/inequality
				if (peek(1) == '=')
				{
					//- <=
					literal = C_LITERAL_SMALLER_EQUAL;
					type = token_type_smaller_equal;
				}
				else
				{
					//- <
					literal = C_LITERAL_SMALLER;
					type = token_type_smaller;
				}
				break;
			}
			case '>':
			{
				//- detect composite equality/inequality
				if (peek(1) == '=')
				{
					//- >=
					literal = C_LITERAL_GREATER_EQUAL;
					type = token_type_greater_equal;
				}
				else
				{
					//- >
					literal = C_LITERAL_GREATER;
					type = token_type_greater;
				}
				break;
			}
			default:
			{
				//- report error
				literal = C_ERROR_UNRECOGNIZED_LITERAL;
				break;
			}
			}

			//- ignore processed literal
			advance();

			return make_token(m_cursor.m_line, literal, type);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccompiler::sscanner::process_token(stoken&& token)
		{
			m_token_stream.m_stream.emplace_back(std::move(token));
		}

		//------------------------------------------------------------------------------------------------------------------------
		char ccompiler::sscanner::peek(uint32_t lookahead /*= 0*/) const
		{
			SLANG_ASSERT(m_cursor.m_current + lookahead < m_code.size() + 1, "Invalid operation. Index out of bound");
			return m_code[m_cursor.m_current + lookahead];
		}

		//------------------------------------------------------------------------------------------------------------------------
		slang::compile_result ccompiler::sscanner::scan(stringview_t code)
		{
			ZoneScopedN("ccompiler::sscanner::scan");

			m_code = code;

			for (;;)
			{
				auto token = next_token();

				if (token.m_type == token_type_eof ||
					token.m_type == token_type_error)
				{
					//- end of source or error. Move the last know token and stop
					process_token(std::move(token));
					break;
				}

				process_token(std::move(token));
			}

			slang_print(log_level_debug, true, debug::print_token_stream(m_token_stream).c_str());

			return m_result;
		}

		//- compiler expects a null terminated string
		//------------------------------------------------------------------------------------------------------------------------
		compile_result ccompiler::compile(stringview_t code)
		{
			ZoneScopedN("ccompiler::compile");

			reset();

			//- proceed with token compilation if we have scanned successfully
			if (m_result = m_scanner.scan(code); m_result == compile_result_ok)
			{
				m_result = m_compiler.compile(std::move(m_scanner.m_token_stream));
			}
			return m_result;
		}

		//------------------------------------------------------------------------------------------------------------------------
		slang::compile_result ccompiler::scompiler::compile(stoken_stream&& stream)
		{
			ZoneScopedN("ccompiler::scompiler::compile");

			m_cursor.m_stream = std::move(stream);

			//- TODO: maybe reconsider how we should iterate, unsure at this point.
			//- We do not need to process the last token, it is expected to be 'eof'
			for (m_cursor.m_current = 0u; m_cursor.m_current < m_cursor.m_stream.m_stream.size() - 1; ++m_cursor.m_current)
			{
				if (peek().m_type == token_type_error)
				{
					emit_error_at_token();
					break;
				}

				process_current_token();
			}

			//- finalize compilation
			consume(token_type_eof, C_ERROR_EOF_EXPECTED);
			emit_byte(opcode_return, m_cursor.m_stream.m_stream.back().m_line + 1);

			return m_result;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccompiler::scompiler::process_current_token()
		{
			const auto& token = peek();

			switch (token.m_type)
			{
			case token_type_number:
			{
				number();
				break;
			}
			case token_type_null:
			case token_type_true:
			case token_type_false:
			{
				literal();
				break;
			}
			//- prefix tokens
			case token_type_left_bracket:
			{
				break;
			}
			case token_type_left_brace:
			{
				break;
			}
			case token_type_left_paren:
			{
				break;
			}

			default:
			{
				//- report unknown token type
				break;
			}
			}

			slang_print(log_level_debug, true, debug::print_chunk(m_chunk).c_str());
		}

		//------------------------------------------------------------------------------------------------------------------------
		slang::detail::stoken& ccompiler::scompiler::peek(uint32_t lookahead/* = 0*/)
		{
			SLANG_ASSERT(m_cursor.m_current + lookahead < m_cursor.m_stream.m_stream.size() + 1, "Invalid operation. Index out of bound");
			return m_cursor.m_stream.m_stream[m_cursor.m_current + lookahead];
		}

		//- Consume the next token if it has the expected type, meaning we acknowledge it and just skip,
		//- if the type is not the expected one, then we raise an error
		//------------------------------------------------------------------------------------------------------------------------
		void ccompiler::scompiler::consume(token_type expected, stringview_t error_message)
		{
			const auto& token = peek();

			if (token.m_type == expected)
			{
				++m_cursor.m_current;
				return;
			}
			emit_error(m_cursor.m_current, error_message);
		}

		//- Writes something into the opcode array, the next write will be on the next index
		//------------------------------------------------------------------------------------------------------------------------
		void ccompiler::scompiler::emit_byte(byte_t byte, uint32_t line)
		{
			m_chunk.write_opcode(byte, line);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccompiler::scompiler::emit_error_at_current_token()
		{
			const auto& token = peek();

			emit_error(token.m_line, token.m_text.c_str());
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccompiler::scompiler::emit_error(uint32_t line, stringview_t message)
		{
			slang_print(log_level_error, true,
				fmt::format("Error at line {}: '{}'", line, message.data()).c_str());
			m_result = compile_result_fail;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccompiler::scompiler::number()
		{
			const auto& token = peek();

			emit_constant<float>(std::stof(token.m_text.c_str()), token.m_line);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccompiler::scompiler::literal()
		{
			const auto& token = peek();
			auto opcode = opcode_noop;

			switch (token.m_type)
			{
			case token_type_true:
			{
				opcode = opcode_true;
				break;
			}
			case token_type_false:
			{
				opcode = opcode_false;
				break;
			}
			case token_type_null:
			{
				opcode = opcode_null;
				break;
			}
			default:
				break;
			}

			emit_byte(opcode, token.m_line);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccompiler::scompiler::expression()
		{
			parse_precedence(precedence_type_assignment);

			consume(token_type_semicolon, "Expected ';' after expression");

			//- TODO: emit instruction to pop computed value from VM stack
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccompiler::scompiler::binary()
		{
			const auto& previous = peek(-1);
			const auto type = previous.m_type;

		}

		//- Starting from current token, parses expressions at given precedence level or higher
		//------------------------------------------------------------------------------------------------------------------------
		void ccompiler::scompiler::parse_precedence(precedence_type type)
		{

		}

		//------------------------------------------------------------------------------------------------------------------------
		ccompiler::scompiler::sparse_rule ccompiler::scompiler::parse_rule(token_type type)
		{
			switch (type)
			{
				//-									Prefix		Infix		Precedence
			case token_type_left_paren:		return { nullptr, nullptr, precedence_type_call };
			case token_type_dot:			return { nullptr, nullptr, precedence_type_none };
			case token_type_minus:			return { nullptr, nullptr, precedence_type_none };
			case token_type_plus:			return { nullptr, nullptr, precedence_type_none };
			case token_type_star:			return { nullptr, nullptr, precedence_type_none };
			case token_type_slash:			return { nullptr, nullptr, precedence_type_none };
			case token_type_exclamation:	return { nullptr, nullptr, precedence_type_none };
			case token_type_not_equality:	return { nullptr, nullptr, precedence_type_none };
			case token_type_smaller:		return { nullptr, nullptr, precedence_type_none };
			case token_type_greater:		return { nullptr, nullptr, precedence_type_none };
			case token_type_equality:		return { nullptr, nullptr, precedence_type_none };
			case token_type_smaller_equal:	return { nullptr, nullptr, precedence_type_none };
			case token_type_greater_equal:	return { nullptr, nullptr, precedence_type_none };
			case token_type_identifier:		return { nullptr, nullptr, precedence_type_none };
			case token_type_number:			return { nullptr, nullptr, precedence_type_none };
			case token_type_string:			return { nullptr, nullptr, precedence_type_none };
			case token_type_true:			return { nullptr, nullptr, precedence_type_none };
			case token_type_false:			return { nullptr, nullptr, precedence_type_none };
			case token_type_this:			return { nullptr, nullptr, precedence_type_none };

			case token_type_none:			return { nullptr, nullptr, precedence_type_none };
			case token_type_eof:			return { nullptr, nullptr, precedence_type_none };
			case token_type_error:			return { nullptr, nullptr, precedence_type_none };
			case token_type_equal:			return { nullptr, nullptr, precedence_type_none };
			case token_type_colon:			return { nullptr, nullptr, precedence_type_none };
			case token_type_semicolon:		return { nullptr, nullptr, precedence_type_none };
			case token_type_comma:			return { nullptr, nullptr, precedence_type_none };
			case token_type_left_bracket:	return { nullptr, nullptr, precedence_type_none };
			case token_type_right_bracket:	return { nullptr, nullptr, precedence_type_none };
			case token_type_left_brace:		return { nullptr, nullptr, precedence_type_none };
			case token_type_right_brace:	return { nullptr, nullptr, precedence_type_none };
			case token_type_right_paren:	return { nullptr, nullptr, precedence_type_none };
			case token_type_null:			return { nullptr, nullptr, precedence_type_none };
			case token_type_class:			return { nullptr, nullptr, precedence_type_none };
			case token_type_def:			return { nullptr, nullptr, precedence_type_none };
			case token_type_return:			return { nullptr, nullptr, precedence_type_none };
			case token_type_var:			return { nullptr, nullptr, precedence_type_none };
			default:
				break;
			}

			SLANG_ASSERT(false, "Invalid operation. Unknown token type");
			return { nullptr, nullptr, precedence_type_none };
		}

	} //- detail

} //- slang