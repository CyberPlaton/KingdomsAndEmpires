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

						raylib::DrawText("Text", 5, 5, 20, raylib::BLACK);
						raylib::DrawLine(0, 0, 256, 256, raylib::GREEN);
						raylib::DrawLine(-1024, -10, 1024, 10, raylib::YELLOW);

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
								const auto& transform = *e.get<ecs::stransform>();

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

	class cscenedebugrender_system : public ecs::csystem<ecs::stransform>
	{
	public:
		cscenedebugrender_system(flecs::world& w) :
			ecs::csystem<ecs::stransform>
			(w, "Debug Scene Render System")
		{

			build([&](flecs::entity e, const ecs::stransform& transform)
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

							const auto [p, s, _] = math::transform({ transform.m_x, transform.m_y }, { transform.m_w, transform.m_h },
								{ 0.0f, 0.0f }, transform.m_rotation);

							raylib::DrawRectangleLines(p.x, p.y, s.x, s.y, raylib::MAROON);
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