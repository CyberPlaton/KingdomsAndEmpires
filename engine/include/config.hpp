#pragma once
#if defined (_WIN32) || defined(_WIN64) || defined (__CYGWIN__)
	#ifdef engine_EXPORTS
		#ifdef __GNUC__
			#define ENGINE_API __attribute__ ((dllexport))
		#else
			#define ENGINE_API __declspec(dllexport)
		#endif
	#else
		#ifdef __GNUC__
			#define ENGINE_API __attribute__ ((dllimport))
		#else
			#define ENGINE_API __declspec(dllimport)
		#endif
	#endif
#else
	#if __GNUC__ >= 4
		#define ENGINE_API __attribute__ ((visibility ("default")))
	#else
		#define ENGINE_API
	#endif
#endif