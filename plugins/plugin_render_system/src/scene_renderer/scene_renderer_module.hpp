#pragma once
#include <engine.h>

namespace render_system
{
	//------------------------------------------------------------------------------------------------------------------------
	class cscenerender_system final : public ecs::ctask
	{
	public:
		cscenerender_system(flecs::world& w) :
			ecs::ctask(w, "Scene Render System")
		{
			build([&](float dt)
				{
					ZoneScopedN("Scene Render System");

					if (ecs::cworld_manager::instance().has_active())
					{
						sm::crenderpath path(0);

						const auto& w = ecs::cworld_manager::instance().active();

						//- match at most one entity, that means the one is the active camera
						if (auto e = w.qm().query_one<ecs::scamera>([](ecs::scamera& c)
							{
								return c.m_active == true;
							}); e.is_valid())
						{
							const auto& c = *e.get<ecs::scamera>();

							path.begin_camera({ c.m_position.x, c.m_position.y }, c.m_offset, c.m_rotation, c.m_zoom);
							path.viewrect(c.m_viewrect.x(), c.m_viewrect.y(), c.m_viewrect.w(), c.m_viewrect.h());

							for (auto e : w.visible_entities())
							{
								const auto& sprite = *e.get<ecs::ssprite>();
								const auto& transform = *e.get<ecs::stransform>();

								const vec2_t position = { transform.m_x, transform.m_y };
								const vec2_t scale = { transform.m_w, transform.m_h };

								const auto [p, s, r] = math::transform(position, scale, { 0.0f, 0.0f }, transform.m_rotation);

								for (const auto& mat : sprite.m_materials)
								{
									path.draw_sprite(p, mat.first, r, s, sprite.m_source_rectangle,
										sprite.m_tint, sprite.m_flipx, sprite.m_flipy);
								}
							}
						}
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