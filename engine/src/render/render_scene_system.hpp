#pragma once
#include "../ecs/ecs.hpp"

namespace render
{
	void scene_frame_begin_system();

	//- Class specifying the entry for scene rendering systems. It does not match any components and serves as reference
	//- point for other rendering systems.
	//------------------------------------------------------------------------------------------------------------------------
	struct sframe_begin_system final
	{
		sframe_begin_system(flecs::world& w)
		{
			ecs::system::sconfig cfg{ w };

			cfg.m_name = "Frame Begin System";

			ecs::system::create_task(cfg, scene_frame_begin_system);
		}
	};

	//- 
	//------------------------------------------------------------------------------------------------------------------------
	class cscene_render_module final : public ecs::imodule
	{
	public:
		cscene_render_module(flecs::world& w) : ecs::imodule(w)
		{
			begin<cscene_render_module>("Scene Render Module")
				.subsystem<cscene_render_module, sframe_begin_system>()
			.end<cscene_render_module>();
		}

		RTTR_ENABLE(ecs::imodule);
	};

} //- render