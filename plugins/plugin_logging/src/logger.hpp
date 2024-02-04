#pragma once
#include <plugin_logging.h>

namespace logging
{
	//- 
	//------------------------------------------------------------------------------------------------------------------------
	class clog
	{
	public:
		STATIC_INSTANCE(clog, s_clog);

		bool init(logging_verbosity verbosity);
		void shutdown();

		void trace(stringview_t message);
		void info(stringview_t message);
		void warn(stringview_t message);
		void error(stringview_t message);
		void critical(stringview_t message);
		void debug(stringview_t message);
		float runtime() const;
		float runtime_ms() const;

	private:
		core::ctimer m_timer;
		logging_mode m_mode = logging_mode::logging_mode_console;
		logging_verbosity m_verbosity = logging_verbosity::logging_verbosity_off;
		float m_start_time = 0.0f;
		ref_t< spdlog::logger > m_logger;
	};

} //- logging