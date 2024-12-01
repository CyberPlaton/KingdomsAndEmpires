#include "kingdoms_layer_ui.hpp"
#include <engine.h>
#include <pfd.h>

namespace kingdoms
{
	namespace
	{
		//------------------------------------------------------------------------------------------------------------------------
		struct scontext final
		{
			string_t m_selected_map;
			bool m_selecting_map = false;
		};

		static bool S_OPEN = true;
		scontext S_CTX;

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	bool ckingdoms_layer_ui::init()
	{
// 		S_CTX.m_selected_map = "/maps/integration";
// 		S_CTX.m_selecting_map = true;
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ckingdoms_layer_ui::shutdown()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void ckingdoms_layer_ui::on_update(float dt)
	{
		if (S_CTX.m_selecting_map && !S_CTX.m_selected_map.empty())
		{
			auto* mm = core::cservice_manager::find<cmap_manager>();

			auto result = mm->load_sync("Map", S_CTX.m_selected_map);

			S_CTX.m_selecting_map = false;
			S_CTX.m_selected_map.clear();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ckingdoms_layer_ui::on_world_render()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ckingdoms_layer_ui::on_ui_render()
	{
		ImGui::ShowDemoWindow(nullptr);

		S_OPEN = true;

		vec2_t position;
		vec2_t offset;
		float zoom;
		float rotation;
		core::srect world_rect;

		if (ecs::cworld_manager::instance().has_active())
		{
			const auto& world = ecs::cworld_manager::instance().active();
			world.query_one<const ecs::scamera>([&](const ecs::scamera& c)
				{
					position = c.m_position;
					offset = c.m_offset;
					zoom = c.m_zoom;
					rotation = c.m_rotation;
					world_rect = world.world_visible_area(c.m_position, c.m_offset, c.m_zoom);
					return true;
				});
		}

		if (const auto windowScope = imgui::cwindow_scope("##debug", &S_OPEN))
		{
			if (ImGui::Button("Load Map..."))
			{
				if (const auto selected = pfd::select_folder("Select Map Folder", core::cfilesystem::cwd().view()).result(); !selected.empty())
				{
					S_CTX.m_selected_map = selected;
					S_CTX.m_selecting_map = true;
				}
			}

			ImGui::Text(fmt::format("Camera Position: {}:{}", position.x, position.y).data());
			ImGui::Text(fmt::format("Camera Offset: {}:{}", offset.x, offset.y).data());
			ImGui::Text(fmt::format("Camera Zoom: {}", zoom).data());
			ImGui::Text(fmt::format("Camera Rotation: {}", rotation).data());
			ImGui::Text(fmt::format("Visible Area: {}:{}:{}:{}", world_rect.x(), world_rect.y(), world_rect.w(), world_rect.h()).data());
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ckingdoms_layer_ui::on_post_update(float dt)
	{
	}

} //- kingdoms