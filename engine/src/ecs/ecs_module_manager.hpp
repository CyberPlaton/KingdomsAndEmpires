#pragma once
#include "ecs_module.hpp"

//#define ECS_MODULE(c) ecs::cmodule_manager::register_module(rttr::type::get<c>())

namespace ecs
{
	//- class responsible for loading and unloading modules for a world. Contains all current active modules.
	//-------------------------------------------------------------------------------------------------------------------------
	class ENGINE_API cmodule_manager final : public iworld_context_holder
	{
	public:
		cmodule_manager() = default;
		~cmodule_manager() = default;

	private:
		umap_t<stringview_t, flecs::entity> m_active_modules;
	};

} //- ecs