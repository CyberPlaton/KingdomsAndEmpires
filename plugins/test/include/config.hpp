#pragma once
#if defined (_WIN32) || defined(_WIN64) || defined (__CYGWIN__)
	#ifdef test_EXPORTS
		#ifdef __GNUC__
			#define TEST_API __attribute__ ((dllexport))
		#else
			#define TEST_API __declspec(dllexport)
		#endif
	#else
		#ifdef __GNUC__
			#define TEST_API __attribute__ ((dllimport))
		#else
			#define TEST_API __declspec(dllimport)
		#endif
	#endif
#else
	#if __GNUC__ >= 4
		#define TEST_API __attribute__ ((visibility ("default")))
	#else
		#define TEST_API
	#endif
#endif