#pragma once
#include "core_config.hpp"
#include "core_timer.hpp"

//------------------------------------------------------------------------------------------------------------------------
#ifdef LOGGING_ENABLE
#define log_trace(message)		logging::clog::instance().trace(message)
#define log_debug(message)		logging::clog::instance().debug(message)
#define log_info(message)		logging::clog::instance().info(message)
#define log_warn(message)		logging::clog::instance().warn(message)
#define log_error(message)		logging::clog::instance().error(message)
#define log_critical(message)	logging::clog::instance().critical(message)
#else
#define log_trace(message)
#define log_debug(message)
#define log_info(message)
#define log_warn(message)
#define log_error(message)
#define log_critical(message)
#endif

namespace logging
{
	//- Central instance responsible for logging messages. Can emit to file or console.
	//------------------------------------------------------------------------------------------------------------------------
	class clog final
	{
	public:
		STATIC_INSTANCE_EX(clog);

		bool init(core::logging_verbosity verbosity);
		void shutdown();

		//- String variants
		void trace(const string_t& message) { trace(message.data()); }
		void info(const string_t& message) { info(message.data()); }
		void warn(const string_t& message) { warn(message.data()); }
		void error(const string_t& message) { error(message.data()); }
		void critical(const string_t& message) { critical(message.data()); }
		void debug(const string_t& message) { debug(message.data()); }

		//- Default variants
		void trace(const char* message);
		void info(const char* message);
		void warn(const char* message);
		void error(const char* message);
		void critical(const char* message);
		void debug(const char* message);

		float runtime() const;
		float runtime_ms() const;

	private:
		ref_t<spdlog::logger> m_logger;
		float m_start_time = 0.0f;
		core::ctimer m_timer;
		core::logging_mode m_mode = core::logging_mode::logging_mode_console;
		core::logging_verbosity m_verbosity = core::logging_verbosity::logging_verbosity_off;
	};

} //- logging