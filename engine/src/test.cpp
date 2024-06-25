#include "test.hpp"
#include <plugin_logging.h>

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	void ctests::run(stest::phase phase)
	{
		for (const auto& tests : m_tests[phase])
		{

		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ctests::set_log(const core::error_report_function_t& log)
	{
		m_log = log;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ctests::add_test(stringview_t test)
	{

	}

} //- engine