#pragma once
#include "rttr/registration.h"
#include "rttr/type.h"
#include "rttr/registration_friend.h"
#include "rttr/library.h"
#include "rttr/registration"
#include "rttr/type"
#include "rttr/registration_friend"

#define CAT_IMPL_(a, b) a##b
#define CAT_(a, b) CAT_IMPL_(a, b)
#ifdef __COUNTER__
#define ANONYMOUS_VARIABLE(str) CAT_(str, __COUNTER__)
#else
#define ANONYMOUS_VARIABLE(str) CAT_(str, __LINE__)
#endif

#define RTTR_REGISTRATION_FRIEND_NON_INTRUSIVE() \
	template <typename T> \
	friend void rttr_auto_register_reflection_function_t(); \
	RTTR_REGISTRATION_FRIEND

//- enable registration for private datamembers
#define REFLECTABLE() RTTR_REGISTRATION_FRIEND_NON_INTRUSIVE()

//- reflect a class inside the header file where it is defined.
//- can easily be used multiples times per header file
#define REFLECT_INLINE(__class) \
	template <typename T> \
	extern void rttr_auto_register_reflection_function_t(); \
	template <> \
	void rttr_auto_register_reflection_function_t<__class>(); \
	static const int ANONYMOUS_VARIABLE(auto_register__) = []() { \
		rttr_auto_register_reflection_function_t<__class>(); \
		return 0; \
	}(); \
	template <> \
	inline void rttr_auto_register_reflection_function_t<__class>()