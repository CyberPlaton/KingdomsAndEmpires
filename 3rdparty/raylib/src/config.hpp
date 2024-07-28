#pragma once
#if defined (_WIN32) || defined(_WIN64) || defined (__CYGWIN__)
	#ifdef raylib_EXPORTS
		#ifdef __GNUC__
			#define RLAPI __attribute__ ((dllexport))
		#else
			#define RLAPI __declspec(dllexport)
		#endif
	#else
		#ifdef __GNUC__
			#define RLAPI __attribute__ ((dllimport))
		#else
			#define RLAPI __declspec(dllimport)
		#endif
	#endif
#else
	#if __GNUC__ >= 4
		#define RLAPI __attribute__ ((visibility ("default")))
	#else
		#define RLAPI
	#endif
#endif