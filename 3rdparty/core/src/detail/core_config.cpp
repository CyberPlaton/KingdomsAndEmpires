#include "core_config.hpp"

namespace core
{
	//------------------------------------------------------------------------------------------------------------------------
	void serror_reporter::set_logger(error_report_function_t&& callback)
	{
		serror_reporter::instance().m_callback = std::move(callback);
	}

} //- core