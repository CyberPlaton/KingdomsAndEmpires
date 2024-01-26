#pragma once
#include <config.hpp>
#include "ecs_module.hpp"

#define ECS_MODULE(c) ecs::cmodule_manager::register_module(rttr::type::get<c>())

namespace ecs
{
	//- class responsible for loading and unloading modules for a world. Contains all current active modules.
	//-------------------------------------------------------------------------------------------------------------------------
	class ENGINE_API cmodule_manager
	{
	public:
		static void register_module(rttr::type type);
		static vector_t<stringview_t> registered_modules();

		cmodule_manager(flecs::world& w);
		~cmodule_manager();

		void init_modules();


	private:
		flecs::world& m_world;
		umap_t<stringview_t, flecs::entity> m_active_modules;
	};

} //- ecs