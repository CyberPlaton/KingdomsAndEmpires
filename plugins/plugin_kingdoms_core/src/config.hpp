#pragma once
#if defined (_WIN32) || defined(_WIN64) || defined (__CYGWIN__)
	#ifdef plugin_kingdoms_core_EXPORTS
		#ifdef __GNUC__
			#define KINGDOMS_CORE_API __attribute__ ((dllexport))
		#else
			#define KINGDOMS_CORE_API __declspec(dllexport)
		#endif
	#else
		#ifdef __GNUC__
			#define KINGDOMS_CORE_API __attribute__ ((dllimport))
		#else
			#define KINGDOMS_CORE_API __declspec(dllimport)
		#endif
	#endif
#else
	#if __GNUC__ >= 4
		#define KINGDOMS_CORE_API __attribute__ ((visibility ("default")))
	#else
		#define KINGDOMS_CORE_API
	#endif
#endif