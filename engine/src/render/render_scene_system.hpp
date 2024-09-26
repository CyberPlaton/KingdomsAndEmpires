#pragma once
#include "../ecs/ecs.hpp"

namespace render
{
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
			multithreaded();
			build([&](const ecs::stransform& transform, const ecs::smaterial& material, const ecs::ssprite_renderer& renderer)
				{
					CORE_NAMED_ZONE("Scene Render System");

					auto& layer = sm::get_layer(renderer.m_layer);
					layer.m_flags = sm::layer_flags_2d;

					auto position = math::extract_translation(transform.m_matrix);

					sm::draw_texture(renderer.m_layer, position, material.m_texture);

					auto m = raylib::GetMousePosition();

					sm::draw_rect(renderer.m_layer, { m.x, m.y }, {64.0f, 64.0f}, core::scolor(255, 150, 255, 255));

					raylib::DrawRectangle(1, 1 ,64, 64, {255, 255, 100, 255});

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