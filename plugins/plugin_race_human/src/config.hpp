#pragma once
#if defined (_WIN32) || defined(_WIN64) || defined (__CYGWIN__)
	#ifdef plugin_race_human_EXPORTS
		#ifdef __GNUC__
			#define HUMAN_API __attribute__ ((dllexport))
		#else
			#define HUMAN_API __declspec(dllexport)
		#endif
	#else
		#ifdef __GNUC__
			#define HUMAN_API __attribute__ ((dllimport))
		#else
			#define HUMAN_API __declspec(dllimport)
		#endif
	#endif
#else
	#if __GNUC__ >= 4
		#define HUMAN_API __attribute__ ((visibility ("default")))
	#else
		#define HUMAN_API
	#endif
#endif