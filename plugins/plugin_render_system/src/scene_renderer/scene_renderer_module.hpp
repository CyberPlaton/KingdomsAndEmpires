#pragma once
#include <engine.h>

namespace render_system
{
	//------------------------------------------------------------------------------------------------------------------------
	class cscene_render_system : public ecs::csystem<ecs::stransform, ecs::ssprite>
	{
	public:
		cscene_render_system(flecs::world& w) :
			ecs::csystem<ecs::stransform, ecs::ssprite>
			(w, "Scene Render System")
		{
			run_on(ecs::system_running_phase_on_world_render);
			multithreaded();
			exclude<ecs::tag::sinvisible>();
			build([&](flecs::entity e, const ecs::stransform& transform, const ecs::ssprite& sprite)
				{
					ZoneScopedN("Scene Render System");

					sm::crenderer renderer;

					//- TODO: we do it like this for now. But intended was to use world().visible_entities()
					//- and render them only. The same approach should be done with animation.
					//-
					//- TODO: also we do not consider hierarchy relationships, but we should, otherwise we will
					//- have errors

					//- perform a transform to world space and submit for rendering

					for (const auto& pair : sprite.m_materials)
					{
						const vec2_t position = { transform.m_x, transform.m_y };
						const vec2_t scale = { transform.m_w, transform.m_h };

						const auto [p, s, r] = math::transform(position, scale, { 0.0f, 0.0f }, transform.m_rotation);

						renderer.draw_sprite(sprite.m_layer, p, pair.first, pair.second, r, s, sprite.m_source_rectangle,
							sprite.m_tint, sprite.m_flipx, sprite.m_flipy);
					}
				});
		}
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cscene_render_module : public ecs::imodule
	{
	public:
		cscene_render_module(flecs::world& w) : ecs::imodule(w)
		{
			begin<cscene_render_module>("Scene Render Module")
				.subsystem<cscene_render_module, cscene_render_system>()
				.end<cscene_render_module>();
		}

		RTTR_ENABLE(ecs::imodule);
	};

} //- render_system

namespace render_system
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(cscene_render_module)
	{
		rttr::registration::class_<cscene_render_module>("cscene_render_module")
			.constructor<flecs::world&>()
			(
				rttr::policy::ctor::as_raw_ptr
			)
			;
	}

} //- render_system