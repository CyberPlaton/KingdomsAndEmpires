#pragma once
#if defined (_WIN32) || defined(_WIN64) || defined (__CYGWIN__)
	#ifdef spdlog_EXPORTS
		#ifdef __GNUC__
			#define SPDLOG_API __attribute__ ((dllexport))
		#else
			#define SPDLOG_API __declspec(dllexport)
		#endif
	#else
		#ifdef __GNUC__
			#define SPDLOG_API __attribute__ ((dllimport))
		#else
			#define SPDLOG_API __declspec(dllimport)
		#endif
	#endif
#else
	#if __GNUC__ >= 4
		#define SPDLOG_API __attribute__ ((visibility ("default")))
	#else
		#define SPDLOG_API
	#endif
#endif