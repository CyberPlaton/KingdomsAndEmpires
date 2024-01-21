#pragma once
#include <plugin_logging.h>

namespace logging
{
	//- 
	//------------------------------------------------------------------------------------------------------------------------
	class LOG_API clog
	{
	public:
		STATIC_INSTANCE(clog, s_clog);

#ifndef NDEBUG
		bool init(logging_verbosity verbosity = logging_verbosity::logging_verbosity_warn);
#else
		bool init(logging_verbosity verbosity = logging_verbosity::logging_verbosity_trace);
#endif
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

#define log_app_time()		logging::clog::instance().runtime()
#define log_app_time_ms()	logging::clog::instance().runtime_ms()
#define log_trace(...)		logging::clog::instance().trace(__VA_ARGS__)
#define log_debug(...)		logging::clog::instance().debug(__VA_ARGS__)
#define log_info(...)		logging::clog::instance().info(__VA_ARGS__)
#define log_warn(...)		logging::clog::instance().warn(__VA_ARGS__)
#define log_error(...)		logging::clog::instance().error(__VA_ARGS__)
#define log_critical(...)	logging::clog::instance().critical(__VA_ARGS__)