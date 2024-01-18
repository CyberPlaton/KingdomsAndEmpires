#pragma once
#if defined (_WIN32) || defined(_WIN64) || defined (__CYGWIN__)
	#ifdef template_EXPORTS
		#ifdef __GNUC__
			#define TEMPLATE_API __attribute__ ((dllexport))
		#else
			#define TEMPLATE_API __declspec(dllexport)
		#endif
	#else
		#ifdef __GNUC__
			#define TEMPLATE_API __attribute__ ((dllimport))
		#else
			#define TEMPLATE_API __declspec(dllimport)
		#endif
	#endif
#else
	#if __GNUC__ >= 4
		#define TEMPLATE_API __attribute__ ((visibility ("default")))
	#else
		#define TEMPLATE_API
	#endif
#endif