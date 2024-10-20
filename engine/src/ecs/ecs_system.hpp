#pragma once
#include <core.h>
#include <flecs.h>
#include "ecs_world_context_holder.hpp"

#define DECLARE_SYSTEM(s) \
s() = default; \
~s() = default; \
s(ecs::cworld* w) : ecs::system::isystem(w) \
{ \
	auto cfg = init(); \
	w->create_system(cfg, callback); \
}

namespace ecs
{
	namespace system
	{
		template<typename... TComps>
		using system_callback_t = void(flecs::entity, TComps...);
		using task_callback_t = void(float);
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

		//------------------------------------------------------------------------------------------------------------------------
		struct isystem
		{
			isystem(cworld* w);
			isystem() = default;
			virtual ~isystem() = default;

			virtual sconfig init() = 0;

			RTTR_ENABLE();
		};

	} //- system

} //- ecs