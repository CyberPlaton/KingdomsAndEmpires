#pragma once
#include "../ecs/ecs.hpp"

namespace render
{
	namespace
	{
		static auto S_PLACEHOLDER_TEXTURE = 0;

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

	//- Class specifying the entry for scene rendering systems. It does not match any components and serves as reference
	//- point for other rendering systems.
	//------------------------------------------------------------------------------------------------------------------------
	class cframe_begin_system final : public ecs::ctask
	{
	public:
		cframe_begin_system(flecs::world& w) :
			ecs::ctask
			(w, "Frame Begin System")
		{
			build([&](float){ /**/});
			run_after(flecs::OnUpdate);
		}
	};

	//- System responsible for rendering sprites in current world.
	//------------------------------------------------------------------------------------------------------------------------
	class cscene_render_system final :
		public ecs::csystem<ecs::stransform, ecs::smaterial, ecs::ssprite_renderer>
	{
	public:
		cscene_render_system(flecs::world& w) :
			ecs::csystem<ecs::stransform, ecs::smaterial, ecs::ssprite_renderer>
			(w, "Scene Render System")
		{
			S_PLACEHOLDER_TEXTURE = algorithm::hash("placeholder");

			multithreaded();
			build([&](const ecs::stransform& transform, const ecs::smaterial& material, const ecs::ssprite_renderer& renderer)
				{
					CORE_NAMED_ZONE("Scene Render System");

					auto position = math::extract_translation(transform.m_matrix);

					sm::draw_texture(renderer.m_layer, position, material.m_texture);

					//draw_texture(transform, material, renderer);
				});
			run_after("Frame Begin System");
		}
	};

	//- 
	//------------------------------------------------------------------------------------------------------------------------
	class cscene_render_module final : public ecs::imodule
	{
	public:
		cscene_render_module(flecs::world& w) : ecs::imodule(w)
		{
			begin<cscene_render_module>("Scene Render Module")
				.subsystem<cscene_render_module, cframe_begin_system>()
				.subsystem<cscene_render_module, cscene_render_system>()
			.end<cscene_render_module>();
		}

		RTTR_ENABLE(ecs::imodule);
	};

} //- render