#pragma once
#include "ecs_system.hpp"

namespace ecs
{
	//- manages all systems for a given world. Note that the managed world is not necessarily the current active one.
	//- here we can execute all kind of system related actions within our world.
	//------------------------------------------------------------------------------------------------------------------------
	class csystem_manager final
	{
	public:
		csystem_manager(flecs::world& w);
		~csystem_manager();

		template<class TSystem>
		ref_t<csystem> create_system();

		flecs::system system(stringview_t name) const;

	private:
		flecs::world& m_managed_world;
		vector_t<ref_t<csystem>> m_systems;

	private:
		inline flecs::world& w() { return m_managed_world; }
		inline const flecs::world& w() const { return m_managed_world; }
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<class TSystem>
	ref_t<csystem> ecs::csystem_manager::create_system()
	{
		static_assert(std::is_base_of<csystem, TSystem>::Value, "TSystem must be derived from csystem base class");

		auto s = std::make_shared<TSystem>(w());

		return m_systems.emplace_back(std::move(s));
	}

} //- ecs