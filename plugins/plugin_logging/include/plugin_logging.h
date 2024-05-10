#pragma once
#include <core.h>

namespace logging
{
#if DEBUG
	bool init(core::logging_verbosity verbosity = core::logging_verbosity::logging_verbosity_trace);
#else
	bool init(core::logging_verbosity verbosity = core::logging_verbosity::logging_verbosity_error);
#endif
	void shutdown();
	float app_runtime();
	float app_runtime_ms();

	void log_trace(const char* message);
	void log_trace(stringview_t message);
	void log_trace(const string_t& message);

	void log_debug(const char* message);
	void log_debug(stringview_t message);
	void log_debug(const string_t& message);

	void log_info(const char* message);
	void log_info(stringview_t message);
	void log_info(const string_t& message);

	void log_warn(const char* message);
	void log_warn(stringview_t message);
	void log_warn(const string_t& message);

	void log_error(const char* message);
	void log_error(stringview_t message);
	void log_error(const string_t& message);

	void log_critical(const char* message);
	void log_critical(stringview_t message);
	void log_critical(const string_t& message);

} //- logging