#pragma once
#include "ecs_system.hpp"

namespace ecs
{
	//- manages all systems for a given world. Note that the managed world is not necessarily the current active one.
	//- here we can execute all kind of system related actions within our world.
	//------------------------------------------------------------------------------------------------------------------------
	class csystem_manager final : public iworld_context_holder
	{
	public:
		csystem_manager(flecs::world& w);
		~csystem_manager();

		template<class TSystem>
		ref_t<csystem> create_system();

		flecs::system system(stringview_t name) const;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<class TSystem>
	ref_t<csystem> ecs::csystem_manager::create_system()
	{
		static_assert(std::is_base_of<csystem, TSystem>::Value, "TSystem must be derived from csystem base class");

		auto s = std::make_shared<TSystem>(world());

		return s;
	}

} //- ecs