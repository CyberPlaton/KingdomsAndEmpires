#pragma once
#include "ecs_system.hpp"

namespace ecs
{
	//- manages all systems for a given world. Note that the managed world is not necessarily the current active one.
	//- here we can execute all kind of system related actions within our world.
	//------------------------------------------------------------------------------------------------------------------------
	class csystem_manager final : public iworld_context_holder
	{
		struct ssystem_info
		{
			std::string m_system;
			vector_t<ssystem_info> m_dependencies;
			vector_t<ssystem_info> m_subsystems;
		};

	public:
		csystem_manager(flecs::world& w);
		~csystem_manager();

		template<class TSystem>
		ref_t<csystem> create_system();

		flecs::system system(stringview_t name) const;
		ssystem_info system_info(stringview_t name) const;

	private:
		umap_t<unsigned, ssystem_info> m_system_info;
		umap_t<unsigned, ref_t<csystem>> m_systems;
		
	private:
		void store_system_info(ref_t<csystem> system);
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<class TSystem>
	ref_t<csystem> ecs::csystem_manager::create_system()
	{
		static_assert(std::is_base_of<csystem, TSystem>::Value, "TSystem must be derived from csystem base class");

		auto s = std::make_shared<TSystem>(world());

		auto h = algorithm::hash(s->name());

		store_system_info(s);

		return m_systems[h] = std::move(s);
	}

} //- ecs