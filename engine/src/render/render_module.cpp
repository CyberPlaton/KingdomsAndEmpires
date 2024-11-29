#include "render_module.hpp"
#include "../ecs/ecs_world.hpp"

namespace render
{
	namespace
	{
		static auto S_PLACEHOLDER_TEXTURE = algorithm::hash("placeholder");

		//------------------------------------------------------------------------------------------------------------------------
		void draw_texture(const ecs::stransform& transform, const ecs::smaterial& material, const ecs::ssprite_renderer& renderer)
		{
			using namespace sm;

			const auto& tm = *core::cservice_manager::find<ctexture_manager>();
			const auto& sm = *core::cservice_manager::find<cshader_manager>();

			//- texture and dimension
			const auto& _texture = tm.at(material.m_texture == C_INVALID_HANDLE ? S_PLACEHOLDER_TEXTURE : material.m_texture);
			const auto w = _texture.w();
			const auto h = _texture.h();

			//- construct rectangles for where to sample from and where to draw
			raylib::Rectangle src = { renderer.m_source_rect.x(), renderer.m_source_rect.y(),
				renderer.m_source_rect.w() * w, renderer.m_source_rect.h() * h };
			raylib::Rectangle dst = { transform.m_position.x, transform.m_position.y, transform.m_scale.x * w, transform.m_scale.y * h };
			raylib::Vector2 orig = { 0.0f, 0.0f };

			raylib::DrawTexturePro(_texture.texture(), src, dst, orig, transform.m_rotation, renderer.m_tint.cliteral<raylib::Color>());
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	void scene_render_system(flecs::entity e, const ecs::stransform& transform, const ecs::smaterial& material, const ecs::ssprite_renderer& renderer)
	{
		CORE_ZONE;

		sm::draw_texture(renderer.m_layer, transform.m_position, material.m_texture,
			renderer.m_tint, transform.m_rotation, transform.m_scale, material.m_program, material.m_renderstate, renderer.m_origin, renderer.m_source_rect);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void scene_debug_render_system(flecs::entity e, const ecs::stransform& transform, const ecs::smaterial& material, const ecs::ssprite_renderer& renderer)
	{
		CORE_ZONE;

		const auto& tm = *core::cservice_manager::find<sm::ctexture_manager>();
		const auto& _texture = tm.at(material.m_texture == sm::C_INVALID_HANDLE ? S_PLACEHOLDER_TEXTURE : material.m_texture);
		const auto w = _texture.w();
		const auto h = _texture.h();

		sm::draw_rect(sm::get_layer_debug().m_id, transform.m_position,
			{ transform.m_scale.x * renderer.m_source_rect.w() * w - renderer.m_origin.x * 0.5f,
			transform.m_scale.y * renderer.m_source_rect.h() * h - renderer.m_origin.y * 0.5f },
			core::scolor(core::common_color_red));
	}

	//------------------------------------------------------------------------------------------------------------------------
	srender_frame_begin_system::srender_frame_begin_system(ecs::cworld* w)
	{
		w->create_task(config(), [](float dt)
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
		w->create_task(config(), [](float dt)
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
		cfg.m_flags = ecs::system::system_flag_immediate;

		return cfg;
	}

	//------------------------------------------------------------------------------------------------------------------------
	srender_system::srender_system(ecs::cworld* w)
	{
		w->create_system(config(), &scene_render_system);
	}

	//------------------------------------------------------------------------------------------------------------------------
	ecs::system::sconfig srender_system::config()
	{
		ecs::system::sconfig cfg;

		cfg.m_name = "srender_system";
		cfg.m_run_after = { "srender_frame_begin_system" };
		cfg.m_run_before = { "srender_frame_end_system" };
		cfg.m_flags = ecs::system::system_flag_immediate;

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
		cfg.m_systems = { "srender_frame_begin_system", "srender_frame_end_system", "srender_system"/*, "sdebug_render_system"*/ };
		cfg.m_modules = {};

		return cfg;
	}

} //- render