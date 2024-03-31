#pragma once
#include "slang_config.hpp"

namespace slang
{
	//- Customizable minimal OS interface
	//------------------------------------------------------------------------------------------------------------------------
	struct sos_api
	{
		static inline sos_api& instance() { static sos_api s_os_api; return s_os_api; }

		detail::sallocator m_allocator;
		detail::slogger m_logger;
	};

	static void slang_init_os_api();
	static detail::sallocator& slang_allocator();
	static detail::slogger& slang_logger();

} //- slang