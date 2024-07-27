#pragma once
#if defined (_WIN32) || defined(_WIN64) || defined (__CYGWIN__)
	#ifdef plugin_camera_system_EXPORTS
		#ifdef __GNUC__
			#define CAMERA_API __attribute__ ((dllexport))
		#else
			#define CAMERA_API __declspec(dllexport)
		#endif
	#else
		#ifdef __GNUC__
			#define CAMERA_API __attribute__ ((dllimport))
		#else
			#define CAMERA_API __declspec(dllimport)
		#endif
	#endif
#else
	#if __GNUC__ >= 4
		#define CAMERA_API __attribute__ ((visibility ("default")))
	#else
		#define CAMERA_API
	#endif
#endif