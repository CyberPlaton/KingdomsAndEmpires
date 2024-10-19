#include "scene_renderer_module.hpp"

namespace render_system
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

			//- extract transform data
			auto position = math::extract_translation(transform.m_matrix);
			auto scale = math::extract_scale(transform.m_matrix);
			auto shear = math::extract_shear(transform.m_matrix);
			auto rotation = math::extract_rotation(transform.m_matrix);

			//- texture and dimension
			const auto& _texture = tm.at(material.m_texture == C_INVALID_HANDLE ? S_PLACEHOLDER_TEXTURE : material.m_texture);
			const auto w = _texture.w();
			const auto h = _texture.h();

			//- construct rectangles for where to sample from and where to draw
			raylib::Rectangle src = { renderer.m_source_rect.x(), renderer.m_source_rect.y(),
				renderer.m_source_rect.w() * w, renderer.m_source_rect.h() * h };
			raylib::Rectangle dst = { position.x, position.y, scale.x * w, scale.y * h };
			raylib::Vector2 orig = { 0.0f, 0.0f };

			raylib::DrawTexturePro(_texture.texture(), src, dst, orig, rotation, renderer.m_tint.cliteral<raylib::Color>());
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	void scene_render_system(const ecs::stransform& transform, const ecs::smaterial& material, const ecs::ssprite_renderer& renderer)
	{
		CORE_NAMED_ZONE("Scene Render System");

		auto position = math::extract_translation(transform.m_matrix);

		sm::draw_texture(renderer.m_layer, position, material.m_texture);

		//draw_texture(transform, material, renderer);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void scene_debug_render_system(const ecs::stransform& transform, const ecs::smaterial& material, const ecs::ssprite_renderer& renderer)
	{
		CORE_NAMED_ZONE("Scene Debug Render System");
	}

} //- render_system