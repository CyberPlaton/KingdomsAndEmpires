#include "render_module.hpp"
#include "../ecs/ecs_world.hpp"

namespace render
{
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
				CORE_ZONE;

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

					sm::renderpass_begin(pass);

					for (const auto& id : sorted_entities.at(pass->m_cfg.m_id))
					{
						const auto e = flecs::entity(id);

						const auto* transform = e.get<ecs::stransform>();
						const auto* mesh = e.get<ecs::smesh>();
						const auto* material = e.get<ecs::smaterial>();

						const auto& mtx = math::transform(transform->m_position, transform->m_scale, transform->m_shear, transform->m_rotation);

						sm::draw_mesh(glm::value_ptr(mtx), mesh->m_mesh, material->m_material);
					}

					sm::renderpass_end(pass);
				}
			});
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