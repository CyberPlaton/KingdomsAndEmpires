#pragma once
#if defined (_WIN32) || defined(_WIN64) || defined (__CYGWIN__)
	#ifdef test2_EXPORTS
		#ifdef __GNUC__
			#define TEST2_API __attribute__ ((dllexport))
		#else
			#define TEST2_API __declspec(dllexport)
		#endif
	#else
		#ifdef __GNUC__
			#define TEST2_API __attribute__ ((dllimport))
		#else
			#define TEST2_API __declspec(dllimport)
		#endif
	#endif
#else
	#if __GNUC__ >= 4
		#define TEST2_API __attribute__ ((visibility ("default")))
	#else
		#define TEST2_API
	#endif
#endif