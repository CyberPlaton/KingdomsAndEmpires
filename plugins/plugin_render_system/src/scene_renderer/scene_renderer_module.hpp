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
						const auto& w = ecs::cworld_manager::instance().active();

						sm::crenderpath path(0);

						//- match at most one entity, that means the one is the active camera
						if (auto e = w.qm().query_one<ecs::scamera>([](ecs::scamera& c)
							{
								return c.m_active == true;
							}); e.is_valid())
						{
							const auto& c = *e.get<ecs::scamera>();

							path.begin_camera({ c.m_position.x, c.m_position.y }, c.m_offset, c.m_rotation, c.m_zoom);

							for (auto e : w.visible_entities())
							{
								const auto& sprite = *e.get<ecs::ssprite>();
								auto& transform = *e.get_mut<ecs::stransform>();

								transform.m_rotation += 0.16f;

								const auto [p, s, r] = math::transform({ transform.m_x, transform.m_y }, { transform.m_w, transform.m_h },
									{ 0.0f, 0.0f }, transform.m_rotation);

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

	class cscenedebugrender_system final : public ecs::ctask
	{
	public:
		cscenedebugrender_system(flecs::world& w) :
			ecs::ctask(w, "Debug Scene Render System")
		{

			build([&](float dt)
				{
					if (ecs::cworld_manager::instance().has_active())
					{
						const auto& w = ecs::cworld_manager::instance().active();

						sm::crenderpath path(1);

						if (auto eCamera = w.qm().query_one<ecs::scamera>([](ecs::scamera& c)
							{
								return c.m_active == true;
							}); eCamera.is_valid())
						{
							const auto& c = *eCamera.get<ecs::scamera>();

							path.begin_camera({ c.m_position.x, c.m_position.y }, c.m_offset, c.m_rotation, c.m_zoom);

							for (auto e : w.visible_entities())
							{
								const auto& transform = *e.get<ecs::stransform>();

								const auto [p, s, _] = math::transform({ transform.m_x, transform.m_y }, { transform.m_w, transform.m_h },
									{ 0.0f, 0.0f }, transform.m_rotation);

								core::srect rect = math::caabb(p.x, p.y, s.x / 2.0f, s.y / 2.0f, glm::radians(transform.m_rotation)).to_rect();

								auto tl = rect.top_left();
								auto tr = rect.top_right();
								auto br = rect.bottom_right();
								auto bl = rect.bottom_left();

								raylib::DrawLine(tl.x, tl.y, tr.x, tr.y, raylib::MAROON);
								raylib::DrawLine(tr.x, tr.y, br.x, br.y, raylib::MAROON);
								raylib::DrawLine(br.x, br.y, bl.x, bl.y, raylib::MAROON);
								raylib::DrawLine(bl.x, bl.y, tl.x, tl.y, raylib::MAROON);
							}
						}
					}
				});

			run_after("Scene Render System");
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
				.subsystem<cscene_render_module, cscenedebugrender_system>()
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