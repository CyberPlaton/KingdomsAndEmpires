#include "sm_tiny_shading_language_backend.hpp"

namespace sm
{
	namespace tsl
	{
		namespace detail
		{
			//------------------------------------------------------------------------------------------------------------------------
			ccompiling_context::ccompiling_context() :
				m_result(true), m_cfg(compiling::sconfig{})
			{
			}

			//------------------------------------------------------------------------------------------------------------------------
			void ccompiling_context::init(token_stream_t&& stream, compiling::sconfig cfg /*= {}*/)
			{
				m_stream = std::move(stream);
				m_cfg = cfg;
			}

			//------------------------------------------------------------------------------------------------------------------------
			stoken ccompiling_context::consume(token_type expect, const string_t& message)
			{
				if (peek().m_type == expect)
				{
					return advance();
				}

				emit_error(message);

				return { message, cursor().m_line, token_type_error };
			}

			//------------------------------------------------------------------------------------------------------------------------
			stoken ccompiling_context::consume(token_type expect_range_start, token_type expect_range_end, const string_t& message)
			{
				if (const auto& type = peek().m_type; type >= expect_range_start && type <= expect_range_end)
				{
					return advance();
				}

				emit_error(message);

				return { message, cursor().m_line, token_type_error };
			}

			//------------------------------------------------------------------------------------------------------------------------
			void ccompiling_context::emit_error(const string_t& message)
			{
				result() = false;

				log_error(fmt::format("Compilation error '{}' at line '{}'!",
					message, cursor().m_line));
			}

			//------------------------------------------------------------------------------------------------------------------------
			bool ccompiling_context::is_data_type_token(const stoken& token)
			{
				return token.m_type >= token_type_void && token.m_type <= token_type_sampler2D;
			}

			//------------------------------------------------------------------------------------------------------------------------
			void ccompiling_context::sstack::push(stoken token)
			{
				m_tokens.emplace_back(token);
			}

			//------------------------------------------------------------------------------------------------------------------------
			void ccompiling_context::sstack::pop()
			{
				algorithm::erase_at(m_tokens, m_tokens.end());
			}

			//------------------------------------------------------------------------------------------------------------------------
			void ccompiling_context::sstack::reduce(stoken with_token, unsigned count /*= MAX(unsigned)*/)
			{
				const auto to_pop_count = (count == MAX(unsigned)) ? m_tokens.size() : count;

				for (auto i = 0; i < to_pop_count; ++i)
				{
					pop();
				}

				push(with_token);
			}

		} //- detail

	} //- tsl

} //- sm