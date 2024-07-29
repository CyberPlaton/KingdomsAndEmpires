#include "editor_main_menu_dock.hpp"
#include "../elements/editor_element_context_menu.hpp"

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
	void cmain_menu::on_update(float dt)
	{
		if (!m_material_editor)
		{
			m_material_editor = detail::create_ui_element<cmaterial_editor>(ctx());

			m_material_editor->init();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmain_menu::on_post_update(float dt)
	{
		m_elements_stack.on_post_update(dt);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmain_menu::on_ui_render()
	{
		if (ImGui::BeginMainMenuBar())
		{
			ctx().m_mainmenu_height = ImGui::GetWindowSize().y;

			if (ImGui::BeginMenu("Context Menu"))
			{
				if(ImGui::Button("Popup"))
					ImGui::OpenPopup("##main_menu_context_menu");

				ui::ccontext_menu("##main_menu_context_menu", ctx())
					.begin("Menu")
						.begin("New")
							.item("Project", nullptr, []()
								{
									log_debug(fmt::format("Creating new project"));
								})
						.end()
					.end()
					.draw();

				ImGui::EndMenu();
			}

// 			if (ImGui::BeginMenu("Menu"))
// 			{
// 				if (ImGui::BeginMenu("New"))
// 				{
// 					if (ImGui::MenuItem("Project"))
// 					{
// 						//- Push project creation element
// 					}
// 					ImGui::EndMenu();
// 				}
// 
// 				if (ImGui::MenuItem("Exit"))
// 				{
// 				}
// 				ImGui::EndMenu();
// 			}
// 			if (ImGui::BeginMenu("File"))
// 			{
// 				ImGui::EndMenu();
// 			}
// 			if (ImGui::BeginMenu("Edit"))
// 			{
// 				ImGui::EndMenu();
// 			}
// 			if (ImGui::BeginMenu("Project"))
// 			{
// 				if (ImGui::MenuItem("Docks", nullptr, &ctx().m_docks_enabled))
// 				{
// 				}
// 
// 				ImGui::EndMenu();
// 			}
// 			if (ImGui::BeginMenu("Debug"))
// 			{
// 				ImGui::EndMenu();
// 			}

		}
		ImGui::EndMainMenuBar();

		m_material_editor->on_update(0.016f);
		m_material_editor->on_ui_render();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmain_menu::on_world_render()
	{
		m_elements_stack.on_world_render();
	}

} //- editor