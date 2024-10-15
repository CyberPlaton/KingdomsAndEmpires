#pragma once
#include <engine.h>
#include "../config.hpp"

namespace render_system
{
	void scene_render_system(float dt);
	void scene_debug_render_system(float dt);

	//------------------------------------------------------------------------------------------------------------------------
	struct RENDER_API srender_system final
	{
		srender_system(flecs::world& w)
		{
			ecs::system::sconfig cfg{ w };

			cfg.m_name = "Render System";

			ecs::system::create_task(cfg, scene_render_system);
		}
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct RENDER_API sdebug_render_system final
	{
		sdebug_render_system(flecs::world& w)
		{
			ecs::system::sconfig cfg{ w };

			cfg.m_name = "Debug Render System";

			ecs::system::create_task(cfg, scene_debug_render_system);
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