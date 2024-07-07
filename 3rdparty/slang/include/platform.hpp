#pragma once
//------------------------------------------------------------------------------------------------------------------------
#ifndef CORE_PLATFORM_WINDOWS
	#define CORE_PLATFORM_WINDOWS 0
#endif
#ifndef CORE_PLATFORM_LINUX
	#define CORE_PLATFORM_LINUX 0
#endif
#ifndef CORE_PLATFORM_OSX
	#define CORE_PLATFORM_OSX 0
#endif
#ifndef CORE_PLATFORM_ANDROID
	#define CORE_PLATFORM_ANDROID 0
#endif
#ifndef CORE_PLATFORM_IOS
	#define CORE_PLATFORM_IOS 0
#endif
#ifndef CORE_PLATFORM_NX
	#define CORE_PLATFORM_NX 0
#endif
#ifndef CORE_PLATFORM_XBOXONE
	#define CORE_PLATFORM_XBOXONE 0
#endif
#ifndef CORE_PLATFORM_XBOXSERIES
	#define CORE_PLATFORM_XBOXSERIES 0
#endif
#ifndef CORE_PLATFORM_PS4
	#define CORE_PLATFORM_PS4 0
#endif
#ifndef CORE_PLATFORM_PS5
	#define CORE_PLATFORM_PS5 0
#endif

//------------------------------------------------------------------------------------------------------------------------
#ifndef CORE_PLATFORM_DESKTOP
	#define CORE_PLATFORM_DESKTOP 0
#endif
#ifndef CORE_PLATFORM_MOBILE
	#define CORE_PLATFORM_MOBILE 0
#endif
#ifndef CORE_PLATFORM_CONSOLE
	#define CORE_PLATFORM_CONSOLE 0
#endif
//------------------------------------------------------------------------------------------------------------------------
#ifndef CORE_COMPILER_MSVC
	#define CORE_COMPILER_MSVC 0
#endif
#ifndef CORE_COMPILER_CLANG
	#define CORE_COMPILER_CLANG 0
#endif
#ifndef CORE_COMPILER_GCC
	#define CORE_COMPILER_GCC 0
#endif
//------------------------------------------------------------------------------------------------------------------------
#ifndef CORE_ARCH_64BIT
	#define CORE_ARCH_64BIT 0
#endif
#ifndef CORE_ARCH_32BIT
	#define CORE_ARCH_32BIT 0
#endif
//------------------------------------------------------------------------------------------------------------------------
#ifndef CORE_DEBUG
	#define CORE_DEBUG 0
#endif
#ifndef CORE_RELEASE
	#define CORE_RELEASE 0
#endif
#ifndef CORE_HYBRID
	#define CORE_HYBRID 0
#endif

//- @reference: bx/platform.h
//------------------------------------------------------------------------------------------------------------------------
#if defined(__clang__)
	//- clang defines __GNUC__ or _MSC_VER
	#undef CORE_COMPILER_CLANG
	#define CORE_COMPILER_CLANG (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
#elif defined(_MSC_VER)
	#undef CORE_COMPILER_MSVC
	#define CORE_COMPILER_MSVC _MSC_VER
#elif defined (__GNUC__)
	#undef CORE_COMPILER_GCC
	#define CORE_COMPILER_GCC (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#else
	#error "CORE_COMPILER_* undefined!"
#endif

//------------------------------------------------------------------------------------------------------------------------
#if defined(__x86_64__)    \
 || defined(_M_X64)        \
 || defined(__aarch64__)   \
 || defined(__64BIT__)     \
 || defined(__mips64)      \
 || defined(__powerpc64__) \
 || defined(__ppc64__)     \
 || defined(__LP64__)
	#undef CORE_ARCH_64BIT
	#define CORE_ARCH_64BIT 64
#else
	#undef CORE_ARCH_32BIT
	#define CORE_ARCH_32BIT 32
#endif

//------------------------------------------------------------------------------------------------------------------------
#if defined(_DURANGO) || defined(_XBOX_ONE)
	#undef CORE_PLATFORM_XBOXONE
	#define CORE_PLATFORM_XBOXONE 1
#elif defined(_WIN32) || defined (_WIN64)
	#undef CORE_PLATFORM_WINDOWS
	#define CORE_PLATFORM_WINDOWS 1
#elif defined (__ANDROID__)
	#undef CORE_PLATFORM_ANDROID
	#define CORE_PLATFORM_ANDROID 1
#elif defined (__linux__)
	#undef CORE_PLATFORM_LINUX
	#define CORE_PLATFORM_LINUX 1
#elif defined (__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__)
	#undef CORE_PLATFORM_IOS
	#define CORE_PLATFORM_IOS 1
#elif defined (__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
	#undef CORE_PLATFORM_OSX
	#define CORE_PLATFORM_OSX 1
#elif defined (__ORBIS__)
	#undef CORE_PLATFORM_PS4
	#define CORE_PLATFORM_PS4 1
#elif defined (__PROSPERO__)
	#undef CORE_PLATFORM_PS5
	#define CORE_PLATFORM_PS5 1
#elif defined (__NX__)
	#undef CORE_PLATFORM_NX
	#define CORE_PLATFORM_NX 1
#else
	#error "CORE_PLATFORM_* undefined!"
#endif

//------------------------------------------------------------------------------------------------------------------------
#if defined(CORE_PLATFORM_WINDOWS) || defined(CORE_PLATFORM_LINUX) || defined(CORE_PLATFORM_OSX)
	#undef CORE_PLATFORM_DESKTOP
	#define CORE_PLATFORM_DESKTOP 1
#elif defined(CORE_PLATFORM_XBOXONE) \
	|| defined(CORE_PLATFORM_XBOXSERIES) \
	|| defined(CORE_PLATFORM_PS4) \
	|| defined(CORE_PLATFORM_PS5) \
	|| defined(CORE_PLATFORM_NX)
	#undef CORE_PLATFORM_CONSOLE
	#define CORE_PLATFORM_CONSOLE 1
#elif defined (CORE_PLATFORM_IOS) || defined(CORE_PLATFORM_ANDROID)
	#undef CORE_PLATFORM_MOBILE
	#define CORE_PLATFORM_MOBILE 1
#endif

//------------------------------------------------------------------------------------------------------------------------
#if DEBUG
	#undef CORE_DEBUG
	#define CORE_DEBUG 1
#elif RELEASE
	#undef CORE_RELEASE
	#define CORE_RELEASE 1
#elif HYBRID
	#undef CORE_HYBRID
	#define CORE_HYBRID 1
#endif