#pragma once
#if defined (_WIN32) || defined(_WIN64) || defined (__CYGWIN__)
	#ifdef plugin_status_effects_EXPORTS
		#ifdef __GNUC__
			#define STATUS_EFFECTS_API __attribute__ ((dllexport))
		#else
			#define STATUS_EFFECTS_API __declspec(dllexport)
		#endif
	#else
		#ifdef __GNUC__
			#define STATUS_EFFECTS_API __attribute__ ((dllimport))
		#else
			#define STATUS_EFFECTS_API __declspec(dllimport)
		#endif
	#endif
#else
	#if __GNUC__ >= 4
		#define STATUS_EFFECTS_API __attribute__ ((visibility ("default")))
	#else
		#define STATUS_EFFECTS_API
	#endif
#endif