#pragma once
#include <engine.h>

namespace render_system
{
	//------------------------------------------------------------------------------------------------------------------------
	class ccamera_system final : public ecs::cfree_system
	{
	public:
		ccamera_system(flecs::world& w) :
			ecs::cfree_system(w, "Camera System")
		{
			build([&](float dt)
				{
					//- TODO: there should only be one active camera in the world, maybe we can do something using a singleton
					//- or something like that
					ZoneScopedN("Camera System");

					sm::crenderer renderer;

					const auto* w = world_context<ecs::cworld>();

					auto e = w->qm().query_one<const ecs::scamera>(
						[](const ecs::scamera& c)
						{
							return c.m_active == true;
						});

					const auto& c = *e.get<ecs::scamera>();
					const auto& t = *e.get<ecs::stransform>();

					renderer.begin_camera({ t.m_x, t.m_y }, c.m_offset, t.m_rotation, c.m_zoom);
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

			run_after("Camera System");
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