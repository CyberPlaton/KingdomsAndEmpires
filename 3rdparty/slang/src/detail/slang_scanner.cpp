#include "slang_scanner.hpp"

namespace slang
{
	namespace
	{
		inline static bool is_identifier(char c);
		inline static bool is_eof(char c);
		inline static bool is_digit(char c);
		inline static bool is_whitespace(char c);
		inline static bool is_newline(char c);
		inline static bool is_comment(char c);
		inline static bool is_keyword(stringview_t text);

	} //- unnamed

	namespace detail
	{
		//------------------------------------------------------------------------------------------------------------------------
		cscanning_context::cscanning_context(const char* code, sconfig cfg /*= {}*/) :
			m_code(code), m_cfg(cfg), m_result(compile_result_ok)
		{
		}

	} //- detail

	//------------------------------------------------------------------------------------------------------------------------
	cscanner::cscanner(const char* code, detail::cscanning_context::sconfig cfg /*= {}*/) :
		m_ctx(detail::cscanning_context{ code, cfg })
	{
	}

} //- slang