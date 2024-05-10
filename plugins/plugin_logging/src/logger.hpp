#pragma once
#include <plugin_logging.h>

namespace logging
{
	//- 
	//------------------------------------------------------------------------------------------------------------------------
	class clog
	{
	public:
		STATIC_INSTANCE_EX(clog);

		bool init(core::logging_verbosity verbosity);
		void shutdown();

		void trace(const char* message);
		void info(const char* message);
		void warn(const char* message);
		void error(const char* message);
		void critical(const char* message);
		void debug(const char* message);
		float runtime() const;
		float runtime_ms() const;

	private:
		core::ctimer m_timer;
		core::logging_mode m_mode = core::logging_mode::logging_mode_console;
		core::logging_verbosity m_verbosity = core::logging_verbosity::logging_verbosity_off;
		float m_start_time = 0.0f;
		ref_t<spdlog::logger> m_logger;
	};

} //- logging