#pragma once
#include <plugin_logging.h>

namespace logging
{
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

		template< typename... ARGS >
		void log_trace(ARGS&&... args);
		template< typename... ARGS >
		void log_info(ARGS&&... args);
		template< typename... ARGS >
		void log_warn(ARGS&&... args);
		template< typename... ARGS >
		void log_error(ARGS&&... args);
		template< typename... ARGS >
		void log_critical(ARGS&&... args);
		template< typename... ARGS >
		void log_debug(ARGS&&... args);

	private:

	};

} //- logging