#pragma once
#include <core.h>
#include <flecs.h>
#include "ecs_world_context_holder.hpp"

//- Use macro to reflect your system, the system function must be declared and implemented.
//-------------------------------------------------------------------------------------------------------------------------
#define REGISTER_SYSTEM(s) \
	rttr::cregistrator<s>(STRINGIFY(s)) \
		.ctor<ecs::cworld*>() \
		.meth(ecs::system::ssystem::C_SYSTEM_CONFIG_FUNC_NAME, &s::config) \
		;

//-------------------------------------------------------------------------------------------------------------------------
#define DECLARE_SYSTEM(system_struct, system_function) \
system_struct() = default; \
system_struct(ecs::cworld* w) { w->create_system(config(), system_function); } \
static stringview_t name() { return STRINGIFY(system_struct); } \
static ecs::system::sconfig config(); \
RTTR_ENABLE();

namespace ecs
{
	namespace system
	{
		template<typename... TComps>
		using system_callback_t = void(flecs::entity, TComps...);
		using task_callback_t = void(flecs::world&, float);
		using system_flags_t = int;

		//------------------------------------------------------------------------------------------------------------------------
		enum system_flag : uint8_t
		{
			system_flag_none = 0,
			system_flag_multithreaded,
			system_flag_immediate,
		};

		//------------------------------------------------------------------------------------------------------------------------
		struct sconfig final
		{
			string_t m_name;
			vector_t<string_t> m_run_after;
			vector_t<string_t> m_run_before;
			system_flags_t m_flags = 0;
		};

		//- This is a dummy system to show how one should be defined. While creating you must not inherit from it.
		//- Define all required functions and reflect them to RTTR using the macro REGISTER_SYSTEM().
		//------------------------------------------------------------------------------------------------------------------------
		struct ssystem final
		{
			static constexpr rttr::string_view C_SYSTEM_CONFIG_FUNC_NAME = "config";
			static constexpr array_t<rttr::string_view, 1> C_SYSTEM_FUNC_NAMES = { C_SYSTEM_CONFIG_FUNC_NAME };

			static sconfig config() { return {}; }

			RTTR_ENABLE();
		};

	} //- system

} //- ecs