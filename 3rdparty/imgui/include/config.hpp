#pragma once
#if defined (_WIN32) || defined(_WIN64) || defined (__CYGWIN__)
	#ifdef imgui_EXPORTS
		#ifdef __GNUC__
			#define IMGUI_API __attribute__ ((dllexport))
		#else
			#define IMGUI_API __declspec(dllexport)
		#endif
	#else
		#ifdef __GNUC__
			#define IMGUI_API __attribute__ ((dllimport))
		#else
			#define IMGUI_API __declspec(dllimport)
		#endif
	#endif
#else
	#if __GNUC__ >= 4
		#define IMGUI_API __attribute__ ((visibility ("default")))
	#else
		#define IMGUI_API
	#endif
#endif