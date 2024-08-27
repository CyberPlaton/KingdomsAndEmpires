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
				/*ctx.push_scope();*/

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

				/*ctx.pop_scope();*/
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
// 				if (ctx.closure().type() == closure_type_script)
// 				{
// 					emit_error(ctx, "Cannot 'return' from top-level code");
// 				}

				if (ctx.match_token_type(token_type_semicolon))
				{

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
				/*number();*/
				break;
			}
			case token_type_null:
			case token_type_true:
			case token_type_false:
			{
				
				/*literal_();*/
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

		emit_byte(m_ctx, opcode_return);

		return m_ctx.result() == compile_result_ok;
	}

} //- slang