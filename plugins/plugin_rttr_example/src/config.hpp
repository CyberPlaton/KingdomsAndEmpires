#pragma once
#if defined (_WIN32) || defined(_WIN64) || defined (__CYGWIN__)
	#ifdef plugin_rttr_example_EXPORTS
		#ifdef __GNUC__
			#define RTTR_EXAMPLE_API __attribute__ ((dllexport))
		#else
			#define RTTR_EXAMPLE_API __declspec(dllexport)
		#endif
	#else
		#ifdef __GNUC__
			#define RTTR_EXAMPLE_API __attribute__ ((dllimport))
		#else
			#define RTTR_EXAMPLE_API __declspec(dllimport)
		#endif
	#endif
#else
	#if __GNUC__ >= 4
		#define RTTR_EXAMPLE_API __attribute__ ((visibility ("default")))
	#else
		#define RTTR_EXAMPLE_API
	#endif
#endif