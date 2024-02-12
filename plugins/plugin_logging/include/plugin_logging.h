#pragma once
#include <spdlog.h>
#include <core.h>

namespace logging
{
	//------------------------------------------------------------------------------------------------------------------------
	enum class logging_mode : uint8_t
	{
		logging_mode_console = 0,
		logging_mode_file,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum class logging_verbosity : uint8_t
	{
		logging_verbosity_trace		= SPDLOG_LEVEL_TRACE,	//- internal
		logging_verbosity_debug		= SPDLOG_LEVEL_DEBUG,	//- internal
		logging_verbosity_info		= SPDLOG_LEVEL_INFO,
		logging_verbosity_warn		= SPDLOG_LEVEL_WARN,
		logging_verbosity_error		= SPDLOG_LEVEL_ERROR,
		logging_verbosity_critical	= SPDLOG_LEVEL_CRITICAL,
		logging_verbosity_off		= SPDLOG_LEVEL_OFF,
	};

#if DEBUG
	bool init(logging_verbosity verbosity = logging_verbosity::logging_verbosity_trace);
#else
	bool init(logging_verbosity verbosity = logging_verbosity::logging_verbosity_error);
#endif
	void shutdown();
	float app_runtime();
	float app_runtime_ms();

	void log_trace(stringview_t message);
	void log_trace(const std::string& message);

	void log_debug(stringview_t message);
	void log_debug(const std::string& message);

	void log_info(stringview_t message);
	void log_info(const std::string& message);

	void log_warn(stringview_t message);
	void log_warn(const std::string& message);

	void log_error(stringview_t message);
	void log_error(const std::string& message);

	void log_critical(stringview_t message);
	void log_critical(const std::string& message);

} //- logging