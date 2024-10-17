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
	};

	//------------------------------------------------------------------------------------------------------------------------
	class RENDER_API cscene_render_module : public ecs::imodule
	{
	public:
		cscene_render_module(flecs::world& w) : ecs::imodule(w)
		{
			begin<cscene_render_module>("Scene Render Module")
				.subsystem<cscene_render_module, srender_system>()
				.subsystem<cscene_render_module, sdebug_render_system>()
			.end<cscene_render_module>();
		}

		RTTR_ENABLE(ecs::imodule);
	};

} //- render_system