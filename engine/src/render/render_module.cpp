#include "render_module.hpp"
#include "../ecs/ecs_world.hpp"

namespace render
{
	//------------------------------------------------------------------------------------------------------------------------
	void scene_render_system(flecs::entity e, const ecs::stransform& transform, const ecs::smaterial& material, const ecs::ssprite_renderer& renderer)
	{
		CORE_ZONE;

		const auto* world = reinterpret_cast<ecs::cworld*>(e.world().get_ctx());

// 		if (world->is_visible(e))
// 		{
// 		}
		sm::draw_texture(renderer.m_layer, transform.m_position, material.m_texture,
			renderer.m_tint, transform.m_rotation, transform.m_scale, material.m_program, material.m_renderstate, renderer.m_origin, renderer.m_source_rect);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void scene_debug_render_system(flecs::entity e, const ecs::stransform& transform, const ecs::smaterial& material, const ecs::ssprite_renderer& renderer)
	{
		CORE_ZONE;

// 		if (world->is_visible(e))
// 		{
// 			const auto& tm = *core::cservice_manager::find<sm::ctexture_manager>();
// 			const auto& _texture = tm.at(material.m_texture == sm::C_INVALID_HANDLE ? S_PLACEHOLDER_TEXTURE : material.m_texture);
// 			const auto w = _texture.w();
// 			const auto h = _texture.h();
// 
// 			sm::draw_rect(sm::get_layer_debug().m_id, transform.m_position,
// 				{ transform.m_scale.x * renderer.m_source_rect.w() * w - renderer.m_origin.x * 0.5f,
// 				transform.m_scale.y * renderer.m_source_rect.h() * h - renderer.m_origin.y * 0.5f },
// 				core::scolor(core::common_color_red));
// 		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	srender_frame_begin_system::srender_frame_begin_system(ecs::cworld* w)
	{
		w->create_task(config(), [](flecs::world&, float dt)
			{
				//-- no-op
			});
	}

	//------------------------------------------------------------------------------------------------------------------------
	ecs::system::sconfig srender_frame_begin_system::config()
	{
		ecs::system::sconfig cfg;

		cfg.m_name = "srender_frame_begin_system";
		cfg.m_run_after = { "OnUpdate" };
		cfg.m_flags = ecs::system::system_flag_immediate;

		return cfg;
	}

	//------------------------------------------------------------------------------------------------------------------------
	srender_frame_end_system::srender_frame_end_system(ecs::cworld* w)
	{
		w->create_task(config(), [](flecs::world&, float dt)
			{
				//-- no-op
			});
	}

	//------------------------------------------------------------------------------------------------------------------------
	ecs::system::sconfig srender_frame_end_system::config()
	{
		ecs::system::sconfig cfg;

		cfg.m_name = "srender_frame_end_system";
		cfg.m_run_after = { "srender_frame_begin_system" };
		cfg.m_run_after = { "PreStore" };
		cfg.m_flags = ecs::system::system_flag_immediate;

		return cfg;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sdebug_render_system::sdebug_render_system(ecs::cworld* w)
	{
		w->create_system(config(), &scene_debug_render_system);
	}

	//------------------------------------------------------------------------------------------------------------------------
	ecs::system::sconfig sdebug_render_system::config()
	{
		ecs::system::sconfig cfg;

		cfg.m_name = "sdebug_render_system";
		cfg.m_run_after = { "srender_system" };
		cfg.m_run_before = { "srender_frame_end_system" };
		cfg.m_flags = ecs::system::system_flag_multithreaded;

		return cfg;
	}

	//------------------------------------------------------------------------------------------------------------------------
	srender_system::srender_system(ecs::cworld* w)
	{
		w->create_task(config(), [](flecs::world& ecs, float dt)
			{
				const auto* world = reinterpret_cast<ecs::cworld*>(ecs.get_ctx());

				//- Sort visible entities by their layer, the layer specifies which renderpass is to be used
				const auto& entities = world->visible_entities();
				umap_t<sm::renderpass_id_t, vector_t<flecs::entity_t>> sorted_entities;
				const auto view_mtx = world->camera_view_mtx();
				const auto proj_mtx = world->camera_projection_mtx();

				for (auto& pass : sm::renderpasses())
				{
					pass->m_view_mtx = view_mtx;
					pass->m_projection_mtx = proj_mtx;

					sm::begin_renderpass(pass);

					for (const auto& id : sorted_entities.at(pass->m_cfg.m_id))
					{
						const auto e = flecs::entity(id);

						const auto* transform = e.get<ecs::stransform>();
						const auto* mesh = e.get<ecs::smesh>();
						const auto* material = e.get<ecs::smaterial>();

						const auto& mtx = math::transform(transform->m_position, transform->m_scale, transform->m_shear, transform->m_rotation);

						sm::draw_mesh(glm::value_ptr(mtx), mesh->m_mesh, material->m_material);
					}

					sm::end_renderpass(pass);
				}
			});

		w->create_system(config(), &scene_render_system);
	}

	//------------------------------------------------------------------------------------------------------------------------
	ecs::system::sconfig srender_system::config()
	{
		ecs::system::sconfig cfg;

		cfg.m_name = "srender_system";
		cfg.m_run_after = { "srender_frame_begin_system" };
		cfg.m_run_before = { "srender_frame_end_system" };
		cfg.m_flags = ecs::system::system_flag_multithreaded;

		return cfg;
	}

	//------------------------------------------------------------------------------------------------------------------------
	srender_module::srender_module(ecs::cworld* w)
	{
		w->import_module<srender_module>(config());
	}

	//------------------------------------------------------------------------------------------------------------------------
	ecs::modules::sconfig srender_module::config()
	{
		ecs::modules::sconfig cfg;
		cfg.m_name = "srender_module";
		cfg.m_components = { "stransform", "smaterial", "ssprite_renderer" };
		cfg.m_systems = { "srender_frame_begin_system", "srender_frame_end_system", "srender_system", "sdebug_render_system" };
		cfg.m_modules = {};

		return cfg;
	}

} //- render