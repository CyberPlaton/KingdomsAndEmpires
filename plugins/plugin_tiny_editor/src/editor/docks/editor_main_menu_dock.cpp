#include "editor_main_menu_dock.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	bool cmain_menu::init()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmain_menu::shutdown()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmain_menu::on_ui_render()
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