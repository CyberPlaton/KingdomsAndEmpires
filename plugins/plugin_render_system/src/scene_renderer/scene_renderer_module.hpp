#pragma once
#include <engine.h>

namespace render_system
{
	//------------------------------------------------------------------------------------------------------------------------
	class ccamera_system final : public ecs::csystem<ecs::stransform, ecs::scamera>
	{
	public:
		ccamera_system(flecs::world& w) :
			ecs::csystem<ecs::stransform, ecs::scamera>
			(w, "Camera System")
		{
			build([&](flecs::entity e, const ecs::stransform& transform, const ecs::scamera& camera)
				{
					//- TODO: there should only be one active camera in the world, maybe we can do something using a singleton
					//- or something like that
					ZoneScopedN("Camera System");

					sm::crenderer renderer;

					renderer.use_camera(camera.m_layer, { transform.m_x, transform .m_y }, camera.m_offset, transform.m_rotation, camera.m_zoom);
				});

			run_after(flecs::OnUpdate);
		};
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cscenerender_system final : public ecs::cfree_system
	{
	public:
		cscenerender_system(flecs::world& w) :
			ecs::cfree_system(w, "Scene Render System")
		{
			build([&](float dt)
				{
					ZoneScopedN("Scene Render System");

					sm::crenderer renderer;

					const auto* w = world_context<ecs::cworld>();

					for (auto e : w->visible_entities())
					{
						const auto& sprite = *e.get<ecs::ssprite>();
						const auto& transform = *e.get<ecs::stransform>();

						const vec2_t position = { transform.m_x, transform.m_y };
						const vec2_t scale = { transform.m_w, transform.m_h };

						const auto [p, s, r] = math::transform(position, scale, { 0.0f, 0.0f }, transform.m_rotation);

						renderer.draw_sprite(sprite.m_layer, p, pair.first, pair.second, r, s, sprite.m_source_rectangle,
							sprite.m_tint, sprite.m_flipx, sprite.m_flipy);
					}
				});

			run_after(flecs::OnUpdate);
		}
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cscene_render_module : public ecs::imodule
	{
	public:
		cscene_render_module(flecs::world& w) : ecs::imodule(w)
		{
			begin<cscene_render_module>("Scene Render Module")
				.subsystem<cscene_render_module, ccamera_system>()
				.subsystem<cscene_render_module, cscenerender_system>()
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