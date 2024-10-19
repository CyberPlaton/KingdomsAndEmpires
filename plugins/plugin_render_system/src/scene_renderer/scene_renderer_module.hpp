#pragma once
#include <engine.h>
#include "../config.hpp"

namespace render_system
{
	void scene_render_system(const ecs::stransform& transform, const ecs::smaterial& material, const ecs::ssprite_renderer& renderer);
	void scene_debug_render_system(const ecs::stransform& transform, const ecs::smaterial& material, const ecs::ssprite_renderer& renderer);

	//- System responsible for rendering sprites in current world
	//------------------------------------------------------------------------------------------------------------------------
	struct RENDER_API srender_system final
	{
		srender_system(flecs::world& w)
		{
			ecs::system::sconfig cfg{ w };

			cfg.m_name = "Render System";
			cfg.m_run_after = { "Frame Begin System" };

			ecs::system::create_system(cfg, scene_render_system);
		}

		RTTR_ENABLE();
	};

	//- System responsible for debug rendering on top of rendered sprites in current world
	//------------------------------------------------------------------------------------------------------------------------
	struct RENDER_API sdebug_render_system final
	{
		sdebug_render_system(flecs::world& w)
		{
			ecs::system::sconfig cfg{ w };

			cfg.m_name = "Debug Render System";
			cfg.m_run_after = { "Render System" };

			ecs::system::create_system(cfg, scene_debug_render_system);
		}

		RTTR_ENABLE();
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct RENDER_API srender_module final
	{
		srender_module(flecs::world& w)
		{
			ecs::modules::sconfig cfg{ w };
			cfg.m_name = "Render Module";
			cfg.m_components = { "stransform", "smaterial", "ssprite_renderer"};
			cfg.m_systems = { "srender_system", "sdebug_render_system"};
			cfg.m_modules = {};

			ecs::modules::import_module(cfg);
		}

		RTTR_ENABLE();
	};

} //- render_system