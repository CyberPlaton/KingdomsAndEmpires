#include "editor_dock_bottom_panel.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	bool cbottom_panel::init()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cbottom_panel::shutdown()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cbottom_panel::on_ui_render()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Menu"))
			{
				if (ImGui::MenuItem("Exit"))
				{
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("File"))
			{
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Project"))
			{
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Debug"))
			{
				ImGui::EndMenu();
			}

		}
		ImGui::EndMainMenuBar();
	}

} //- editor