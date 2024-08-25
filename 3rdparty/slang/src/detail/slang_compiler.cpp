#include "slang_compiler.hpp"
#include "slang_debug.hpp"
#include <string.h> //- std::stoi and the like

namespace slang
{
	namespace detail
	{
		namespace
		{
		}; //- unnamed

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

		//- compiler expects a null terminated string
		//------------------------------------------------------------------------------------------------------------------------
		compile_result ccompiler::compile(stringview_t code)
		{
			SLANG_NAMED_CPU_ZONE("ccompiler::compile");

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
			SLANG_NAMED_CPU_ZONE("ccompiler::scompiler::compile");

			m_cursor.m_stream = std::move(stream);

			//- TODO: maybe reconsider how we should iterate, unsure at this point.
			//- We do not need to process the last token, it is expected to be 'eof'
			for (m_cursor.m_current = 0u; m_cursor.m_current < m_cursor.m_stream.m_stream.size() - 1; ++m_cursor.m_current)
			{
				if (peek().m_type == token_type_error)
				{
					emit_error_at_current_token();
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
				literal_();
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
		void ccompiler::scompiler::literal_()
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

		//------------------------------------------------------------------------------------------------------------------------
		void ccompiler::scompiler::variable()
		{

		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccompiler::scompiler::string()
		{

		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccompiler::scompiler::this_()
		{

		}

		//- Starting from current token, parses expressions at given precedence level or higher
		//------------------------------------------------------------------------------------------------------------------------
		void ccompiler::scompiler::parse_precedence(precedence_type type)
		{
			const auto& token = advance();

			auto prefix_rule = parse_rule(token.m_type).m_prefix;

			if (!prefix_rule)
			{
				emit_error_at_current_token();
				return;
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccompiler::scompiler::grouping()
		{

		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccompiler::scompiler::call()
		{

		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccompiler::scompiler::dot()
		{

		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccompiler::scompiler::unary()
		{

		}

		//------------------------------------------------------------------------------------------------------------------------
		ccompiler::scompiler::sparse_rule ccompiler::scompiler::parse_rule(token_type type)
		{
			switch (type)
			{
				//-									Prefix		Infix		Precedence
			case token_type_left_paren:		return sparse_rule(&scompiler::grouping,	&scompiler::call,		precedence_type_call );
			case token_type_dot:			return sparse_rule( nullptr,				&scompiler::dot,		precedence_type_call);
			case token_type_minus:			return sparse_rule(&scompiler::unary,		&scompiler::binary,		precedence_type_term);
			case token_type_plus:			return sparse_rule( nullptr,				&scompiler::binary,		precedence_type_term);
			case token_type_star:			return sparse_rule( nullptr,				&scompiler::binary,		precedence_type_factor);
			case token_type_slash:			return sparse_rule( nullptr,				&scompiler::binary,		precedence_type_factor);
			case token_type_exclamation:	return sparse_rule(&scompiler::unary,		nullptr,				precedence_type_none);
			case token_type_not_equality:	return sparse_rule(nullptr,					&scompiler::binary,		precedence_type_equality);
			case token_type_smaller:		return sparse_rule(nullptr,					&scompiler::binary,		precedence_type_comparison);
			case token_type_greater:		return sparse_rule(nullptr,					&scompiler::binary,		precedence_type_comparison);
			case token_type_equality:		return sparse_rule(nullptr,					&scompiler::binary,		precedence_type_equality);
			case token_type_smaller_equal:	return sparse_rule(nullptr,					&scompiler::binary,		precedence_type_comparison);
			case token_type_greater_equal:	return sparse_rule(nullptr,					&scompiler::binary,		precedence_type_comparison);
			case token_type_identifier:		return sparse_rule(&scompiler::variable,	nullptr,				precedence_type_none);
			case token_type_number:			return sparse_rule(&scompiler::number,		nullptr,				precedence_type_none);
			case token_type_string:			return sparse_rule(&scompiler::string,		nullptr,				precedence_type_none);
			case token_type_true:			return sparse_rule(&scompiler::literal_,	nullptr,				precedence_type_none);
			case token_type_false:			return sparse_rule(&scompiler::literal_,	nullptr,				precedence_type_none);
			case token_type_this:			return sparse_rule(&scompiler::this_,		nullptr,				precedence_type_none);

			case token_type_none:			return sparse_rule(nullptr,					nullptr,				precedence_type_none);
			case token_type_eof:			return sparse_rule(nullptr,					nullptr,				precedence_type_none);
			case token_type_error:			return sparse_rule(nullptr,					nullptr,				precedence_type_none);
			case token_type_equal:			return sparse_rule(nullptr,					nullptr,				precedence_type_none);
			case token_type_colon:			return sparse_rule(nullptr,					nullptr,				precedence_type_none);
			case token_type_semicolon:		return sparse_rule(nullptr,					nullptr,				precedence_type_none);
			case token_type_comma:			return sparse_rule(nullptr,					nullptr,				precedence_type_none);
			case token_type_left_bracket:	return sparse_rule(nullptr,					nullptr,				precedence_type_none);
			case token_type_right_bracket:	return sparse_rule(nullptr,					nullptr,				precedence_type_none);
			case token_type_left_brace:		return sparse_rule(nullptr,					nullptr,				precedence_type_none);
			case token_type_right_brace:	return sparse_rule(nullptr,					nullptr,				precedence_type_none);
			case token_type_right_paren:	return sparse_rule(nullptr,					nullptr,				precedence_type_none);
			case token_type_null:			return sparse_rule(&scompiler::literal_,	nullptr,				precedence_type_none);
			case token_type_class:			return sparse_rule(nullptr,					nullptr,				precedence_type_none);
			case token_type_def:			return sparse_rule(nullptr,					nullptr,				precedence_type_none);
			case token_type_return:			return sparse_rule(nullptr,					nullptr,				precedence_type_none);
			case token_type_var:			return sparse_rule(nullptr,					nullptr,				precedence_type_none);
			default:
				break;
			}

			SLANG_ASSERT(false, "Invalid operation. Unknown token type");
			return { nullptr, nullptr, precedence_type_none };
		}

	} //- detail

} //- slang