#include "slang_compiler.hpp"
#include "slang_error.hpp"
#include "slang_debug.hpp"
#include <string.h> //- std::stoi and the like

namespace slang
{
	namespace
	{
		//- Create a constant value for chunk. Function returns the index where the constant is stored in the vector.
		//------------------------------------------------------------------------------------------------------------------------
		template<typename T>
		inline static byte_t make_constant(detail::ccompiling_context& ctx, T&& value)
		{
			auto& constants = ctx.chunk().constants();
			const auto idx = constants.size();

			SLANG_ASSERT(idx < MAX(byte_t), "Invalid operation. 'Constant' limit for chunk reached");

			constants.emplace_back(cvalue(object_type_number, std::move(value)));

			return static_cast<byte_t>(idx);
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline static void emit_byte(detail::ccompiling_context& ctx, byte_t byte)
		{
			ctx.chunk().write_opcode(byte, ctx.current_line());
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline static byte_t emit_jump(detail::ccompiling_context& ctx, byte_t byte)
		{
			emit_byte(ctx, byte);
			emit_byte(ctx, MAX(byte_t)); //- TODO: clarify what this is for
			emit_byte(ctx, MAX(byte_t));

			return static_cast<byte_t>(ctx.chunk().code().size()) - 2;
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline static void emit_error(detail::ccompiling_context& ctx, stringview_t message)
		{
			if (slogger::instance().m_callback)
			{
				slogger::instance().m_callback(log_level_error, message.data());
			}

			ctx.result() = compile_result_fail;
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline static void emit_loop(detail::ccompiling_context& ctx, const int start)
		{
			emit_byte(ctx, opcode_loop);

			const auto offset = static_cast<int>(ctx.chunk().code().size()) - start + 2;

			if (offset > MAX(byte_t))
			{
				emit_error(ctx, "Looping body is too large");
			}

			emit_byte(ctx, (offset >> 8) & MAX(byte_t));
			emit_byte(ctx, offset >> 8);
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename T>
		inline static void emit_constant(detail::ccompiling_context& ctx, T&& value)
		{
			const auto line = ctx.current_line();
			emit_byte(ctx, opcode_constant, line);
			emit_byte(make_constant<T>(ctx, std::move(value)), line);
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline static void patchup_jump(detail::ccompiling_context& ctx, const int offset)
		{
			const auto jump_count = static_cast<int>(ctx.chunk().code().size());

			if (jump_count > MAX(byte_t))
			{
				emit_error(ctx, "Too long jump instruction!");
			}

			auto& code = ctx.chunk().code();

			code[offset] = (jump_count >> 8) & MAX(byte_t);
			code[offset + 1] = jump_count& MAX(byte_t);
		}

	}; //- unnamed

	namespace detail
	{
		namespace expressions
		{
			//------------------------------------------------------------------------------------------------------------------------
			void primary(detail::ccompiling_context& ctx)
			{

			}

			//------------------------------------------------------------------------------------------------------------------------
			void call(detail::ccompiling_context& ctx)
			{

			}

			//------------------------------------------------------------------------------------------------------------------------
			void unary(detail::ccompiling_context& ctx)
			{

			}

			//------------------------------------------------------------------------------------------------------------------------
			void factor(detail::ccompiling_context& ctx)
			{

			}

			//------------------------------------------------------------------------------------------------------------------------
			void term(detail::ccompiling_context& ctx)
			{

			}

			//------------------------------------------------------------------------------------------------------------------------
			void comparison(detail::ccompiling_context& ctx)
			{

			}

			//------------------------------------------------------------------------------------------------------------------------
			void equality(detail::ccompiling_context& ctx)
			{

			}

			//------------------------------------------------------------------------------------------------------------------------
			void logacal_and(detail::ccompiling_context& ctx)
			{

			}

			//------------------------------------------------------------------------------------------------------------------------
			void logical_or(detail::ccompiling_context& ctx)
			{

			}

			//------------------------------------------------------------------------------------------------------------------------
			void assignment(detail::ccompiling_context& ctx)
			{

			}

			//------------------------------------------------------------------------------------------------------------------------
			void expression(detail::ccompiling_context& ctx)
			{

			}

		} //- expressions

		namespace statements
		{
			//------------------------------------------------------------------------------------------------------------------------
			void statement_expression(detail::ccompiling_context& ctx)
			{
				expressions::expression(ctx);

				if (!ctx.consume(token_type_semicolon))
				{
					emit_error(ctx, "Expected ';' after expression");
				}

				emit_byte(ctx, opcode_pop);
			}

			//------------------------------------------------------------------------------------------------------------------------
			void statement_for(detail::ccompiling_context& ctx)
			{
				ctx.push_scope();

				if (!ctx.consume(token_type_left_paren))
				{
					emit_error(ctx, "Expected '(' after 'for' statement");
				}

				//- For loop initialization
				if (ctx.consume(token_type_semicolon))
				{
				}
				else if (ctx.consume(token_type_var))
				{
					//- Running variable declaration
					rules::declaration_variable(ctx);
				}
				else
				{
					//- Running variable is declared for a statement
					expressions::expression(ctx);
				}

				//- For loop exit condition
				int loop_start = static_cast<int>(ctx.chunk().code().size());
				auto exit_jump = MIN(byte_t);
				if (!ctx.match_token_type(token_type_semicolon))
				{
					expressions::expression(ctx);

					if (!ctx.consume(token_type_semicolon))
					{
						emit_error(ctx, "Expected ';' after 'for' loop condition");
					}

					//- Jump out of the for loop if the condition is false
					exit_jump = emit_jump(ctx, opcode_jump_if_false);

					emit_byte(ctx, opcode_pop);
				}

				//- For loop runner variable incrementation
				if (!ctx.match_token_type(token_type_right_paren))
				{
					const auto body_jump = emit_jump(ctx, opcode_jump);
					const auto increment_start = static_cast<int>(ctx.chunk().code().size());

					expressions::expression(ctx);

					emit_byte(ctx, opcode_pop);

					if (!ctx.consume(token_type_right_paren))
					{
						emit_error(ctx, "Expected ')' after 'for' clauses");
					}

					loop_start = increment_start;

					patchup_jump(ctx, body_jump);
				}

				statements::statement(ctx);
				emit_loop(ctx, loop_start);

				if (exit_jump != MIN(byte_t))
				{
					patchup_jump(ctx, exit_jump);
					emit_byte(ctx, opcode_pop);
				}

				ctx.pop_scope();
			}

			//------------------------------------------------------------------------------------------------------------------------
			void statement_print(detail::ccompiling_context& ctx)
			{
				expressions::expression(ctx);

				if (!ctx.consume(token_type_semicolon))
				{
					emit_error(ctx, "Expected ';' after argument for 'print'");
				}

				emit_byte(ctx, opcode_print);
			}

			//------------------------------------------------------------------------------------------------------------------------
			void statement_return(detail::ccompiling_context& ctx)
			{
				if (ctx.closure().type() == closure_type_script)
				{
					emit_error(ctx, "Cannot 'return' from top-level code");
				}


			}

			//------------------------------------------------------------------------------------------------------------------------
			void statement_if(detail::ccompiling_context& ctx)
			{
				if (!ctx.consume(token_type_left_paren))
				{
					emit_error(ctx, "Expected '(' after 'if'");
				}

				expressions::expression(ctx);

				if (!ctx.consume(token_type_right_paren))
				{
					emit_error(ctx, "Expected ')' after 'if' condition");
				}

				const auto then_jump = emit_jump(ctx, opcode_jump_if_false);

				emit_byte(ctx, opcode_pop);

				statements::statement(ctx);

				const auto else_jump = emit_jump(ctx, opcode_jump);

				patchup_jump(ctx, then_jump);

				emit_byte(ctx, opcode_pop);

				if (ctx.match_token_type(token_type_else))
				{
					statements::statement(ctx);
				}

				patchup_jump(ctx, else_jump);
			}

			//------------------------------------------------------------------------------------------------------------------------
			void statement_while(detail::ccompiling_context& ctx)
			{

			}

			//------------------------------------------------------------------------------------------------------------------------
			void block(detail::ccompiling_context& ctx)
			{

			}

			//------------------------------------------------------------------------------------------------------------------------
			void statement(detail::ccompiling_context& ctx)
			{
				if (ctx.match_token_type(token_type_print))
				{

				}
				else if (ctx.match_token_type(token_type_for))
				{

				}
				else if (ctx.match_token_type(token_type_if))
				{

				}
				else if (ctx.match_token_type(token_type_return))
				{

				}
				else if (ctx.match_token_type(token_type_identifier))
				{

				}
				else if (ctx.match_token_type(token_type_left_brace))
				{

				}
				else
				{
					statement_expression(ctx);
				}
			}

		} //- statements

		namespace rules
		{
			//------------------------------------------------------------------------------------------------------------------------
			void declaration_class(detail::ccompiling_context& ctx)
			{

			}

			//------------------------------------------------------------------------------------------------------------------------
			void declaration_function(detail::ccompiling_context& ctx)
			{

			}

			//------------------------------------------------------------------------------------------------------------------------
			void declaration_variable(detail::ccompiling_context& ctx)
			{

			}

			//------------------------------------------------------------------------------------------------------------------------
			void declaration(detail::ccompiling_context& ctx)
			{
				if (ctx.match_token_type(token_type_class))
				{
					declaration_class(ctx);
				}
				else if (ctx.match_token_type(token_type_def))
				{
					declaration_class(ctx);
				}
				else if (ctx.match_token_type(token_type_var))
				{
					declaration_class(ctx);
				}
				else
				{
					statements::statement(ctx);
				}
			}

		} //- rules

		//------------------------------------------------------------------------------------------------------------------------
		ccompiling_context::ccompiling_context(stoken_stream&& stream, sconfig cfg /*= {}*/) :
			m_cursor({ std::move(stream), 0}), m_cfg(cfg)
		{
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccompiling_context::process_token()
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
		stoken& ccompiling_context::advance()
		{
			return tokens()[++cursor().m_current];
		}

		//------------------------------------------------------------------------------------------------------------------------
		const stoken& ccompiling_context::peek(unsigned lookahead /*= 0*/) const
		{
			SLANG_ASSERT(cursor().m_current + lookahead < tokens().size() + 1, "Invalid operation. Index out of bound");

			return tokens()[cursor().m_current + lookahead];
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool ccompiling_context::consume(token_type expected)
		{
			if (const auto& token = peek(); token.m_type == expected)
			{
				++cursor().m_current; return true;
			}
			return false;
		}

		//------------------------------------------------------------------------------------------------------------------------
		unsigned ccompiling_context::current_line() const
		{
			return peek().m_line;
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool ccompiling_context::match_token_type(token_type type)
		{
			if (!match_token_type(type))
			{
				return false;
			}
			advance();
			return true;
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool ccompiling_context::check_token_type(token_type type) const
		{
			return peek().m_type == type;
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

	//------------------------------------------------------------------------------------------------------------------------
	ccompiler::ccompiler(stoken_stream&& stream, detail::ccompiling_context::sconfig cfg /*= {}*/) :
		m_ctx(detail::ccompiling_context{ std::move(stream), cfg })
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool ccompiler::compile()
	{
		SLANG_NAMED_CPU_ZONE("ccompiler::compile");

		//- TODO: maybe reconsider how we should iterate, unsure at this point.
		//- We do not need to process the last token, it is expected to be 'eof'
		for (m_ctx.cursor().m_current = 0u; m_ctx.cursor().m_current < m_ctx.tokens().size() - 1; ++m_ctx.cursor().m_current)
		{
			if (m_ctx.peek().m_type == token_type_error)
			{
				/*emit_error_at_current_token();*/
				break;
			}

			m_ctx.process_token();
		}

		//- Finalize compilation
		if (!m_ctx.consume(token_type_eof))
		{
			emit_error(m_ctx, serrors::errors()[error_type_missing_expected_eof]);
		}

		emit_byte(m_ctx, opcode_return, m_ctx.tokens().back().m_line + 1);

		return m_ctx.result() == compile_result_ok;
	}

} //- slang