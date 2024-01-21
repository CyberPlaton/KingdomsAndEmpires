#pragma once
#include "config.hpp"
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

	LOG_API void log_app_time();
	LOG_API void log_app_time_ms();
	LOG_API void log_trace();
	LOG_API void log_debug();
	LOG_API void log_info();
	LOG_API void log_warn();
	LOG_API void log_error();
	LOG_API void log_critical();

} //- logging