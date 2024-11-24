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
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ckingdoms_layer_ui::shutdown()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void ckingdoms_layer_ui::on_update(float dt)
	{

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
		}

		if (S_CTX.m_selecting_map && !S_CTX.m_selected_map.empty())
		{
			auto* mm = core::cservice_manager::find<cmap_manager>();

			auto result = mm->load_sync("Map", S_CTX.m_selected_map);

			S_CTX.m_selecting_map = false;
			S_CTX.m_selected_map.clear();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ckingdoms_layer_ui::on_post_update(float dt)
	{

	}

} //- kingdoms