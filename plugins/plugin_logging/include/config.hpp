#pragma once
#if defined (_WIN32) || defined(_WIN64) || defined (__CYGWIN__)
	#ifdef plugin_logging_EXPORTS
		#ifdef __GNUC__
			#define LOG_API __attribute__ ((dllexport))
		#else
			#define LOG_API __declspec(dllexport)
		#endif
	#else
		#ifdef __GNUC__
			#define LOG_API __attribute__ ((dllimport))
		#else
			#define LOG_API __declspec(dllimport)
		#endif
	#endif
#else
	#if __GNUC__ >= 4
		#define LOG_API __attribute__ ((visibility ("default")))
	#else
		#define LOG_API
	#endif
#endif