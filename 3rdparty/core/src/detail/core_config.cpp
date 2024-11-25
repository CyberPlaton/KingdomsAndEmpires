#include "core_config.hpp"

//------------------------------------------------------------------------------------------------------------------------
#if defined(core_EXPORTS)
#if CORE_PLATFORM_WINDOWS && TRACY_ENABLE
void* operator new(unsigned long long n) { return CORE_MALLOC(n); }
void operator delete(void* p) { CORE_FREE(p); }
#endif
#endif

namespace core
{
	//------------------------------------------------------------------------------------------------------------------------
	void serror_reporter::set_logger(error_report_function_t callback)
	{
		serror_reporter::instance().m_callback = callback;
	}

} //- core