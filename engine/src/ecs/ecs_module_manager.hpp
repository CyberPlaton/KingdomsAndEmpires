#pragma once
#include "ecs_module.hpp"

#define ECS_MODULE(c) ecs::cmodule_manager::register_module<c>()

namespace ecs
{
	//- class responsible for loading and unloading modules for a world. Contains all current active modules.
	//-------------------------------------------------------------------------------------------------------------------------
	class cmodule_manager
	{
	public:
		cmodule_manager(flecs::world& w);
		~cmodule_manager();

		template<class TModuleType>
		static void register_module()
		{
			s_registered_modules.insert(rttr::type::get<TModuleType>().get_name().data());
		}


		void init_modules();


	private:
		static uset_t<stringview_t> s_registered_modules;

		flecs::world& m_world;
		umap_t<stringview_t, flecs::entity> m_active_modules;
	};

} //- ecs