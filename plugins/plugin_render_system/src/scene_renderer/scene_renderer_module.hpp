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
					//-
					//- TODO: get active camera from world or something and use it here.
					//- For now we simulate this process by creating one on the fly
					vec2_t p(1.0f);
					vec2_t o(0.0f);
					float r = 0.0f, z = 1.0f;
					core::scolor clear(250, 250, 250, 255);

					uset_t<renderlayer_t> layers;

					for (const auto& pair : sprite.m_materials)
					{
						//- perform a transform to world space and submit for rendering
						const vec2_t position = { transform.m_x, transform.m_y };
						const vec2_t scale = { transform.m_w, transform.m_h };

						const auto [p, s, r] = math::transform(position, scale, { 0.0f, 0.0f }, transform.m_rotation);

						//- TODO: this is really stupid... is to be reworked together with how to process camera type commands
						if (layers.count(sprite.m_layer) == 0)
						{
							renderer.clear_view(sprite.m_layer, clear);
							renderer.use_camera(sprite.m_layer, p, o, r, z);

							layers.insert(sprite.m_layer);
						}

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