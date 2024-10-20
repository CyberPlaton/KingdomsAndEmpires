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
		CORE_NAMED_ZONE("Scene Render System");

		sm::draw_texture(renderer.m_layer, transform.m_position, material.m_texture);

		//draw_texture(transform, material, renderer);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void scene_debug_render_system(flecs::entity e, const ecs::stransform& transform, const ecs::smaterial& material, const ecs::ssprite_renderer& renderer)
	{
		CORE_NAMED_ZONE("Scene Debug Render System");
	}

	//------------------------------------------------------------------------------------------------------------------------
	sdebug_render_system::sdebug_render_system(ecs::cworld* w)
	{
		ecs::system::sconfig cfg;

		cfg.m_name = "Debug Render System";
		cfg.m_run_after = { "Render System" };

		w->create_system(cfg, &scene_debug_render_system);
	}

	//------------------------------------------------------------------------------------------------------------------------
	srender_system::srender_system(ecs::cworld* w)
	{
		ecs::system::sconfig cfg;

		cfg.m_name = "Render System";
		cfg.m_run_after = { "Frame Begin System" };

		w->create_system(cfg, &scene_render_system);
	}

	//------------------------------------------------------------------------------------------------------------------------
	srender_module::srender_module(ecs::cworld* w)
	{
		ecs::modules::sconfig cfg;
		cfg.m_name = "Render Module";
		cfg.m_components = { "stransform", "smaterial", "ssprite_renderer" };
		cfg.m_systems = { "srender_system", "sdebug_render_system" };
		cfg.m_modules = {};

		w->import_module<srender_module>(cfg);
	}

} //- render