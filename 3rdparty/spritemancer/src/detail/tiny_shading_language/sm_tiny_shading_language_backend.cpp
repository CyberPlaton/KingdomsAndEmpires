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
			void ccompiling_context::emit_error(const string_t& message)
			{
				result() = false;

				log_error(fmt::format("Compilation error '{}' at line '{}'!",
					message, cursor().m_line));
			}

		} //- detail

	} //- tsl

} //- sm