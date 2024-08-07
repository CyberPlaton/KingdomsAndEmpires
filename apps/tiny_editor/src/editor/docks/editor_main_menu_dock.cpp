#include "editor_main_menu_dock.hpp"
#include "../elements/editor_element_context_menu.hpp"
#include "../elements/editor_element_dialog.hpp"
#include "../elements/editor_element_text_input.hpp"
#include <pfd.h>

namespace editor
{
	namespace
	{
		constexpr stringview_t C_CREATE_PROJECT_DIALOG_ID = "##createProjectDialog";
		static string_t S_PROJECT_NAME_TEXT_BUFFER;
		static string_t S_PROJECT_PATH_TEXT_BUFFER;
		static bool S_CREATE_PROJECT_WINDOW = false;
		static const char* C_PROJECT_EXTENSION[] = {".project"};

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	bool cmain_menu::init()
	{
		S_PROJECT_NAME_TEXT_BUFFER.resize(128);
		S_PROJECT_PATH_TEXT_BUFFER.resize(256);
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

			//- TODO: the editor main menu or a part of it should be extendable from any part of editor, i.e.
			//- a plugin would like to offer his own menu to set up settings etc., we want to allow this
			//- in an orderly fashion. However, for now we dont do that.

			//- General editor section
			if (const auto editor_scope = imgui::cmenu_scope("Editor"))
			{
				if (const auto new_scope = imgui::cmenu_scope("New"))
				{
					imgui::cmenu_item("Project", &S_CREATE_PROJECT_WINDOW, true);
				}
				if (const auto pref_cope = imgui::cmenu_scope("Preferences"))
				{

				}
				if (const auto exit_scope = imgui::cmenu_scope("Exit"))
				{

				}
			}
			//- Project specific section
			if (const auto project_scope = imgui::cmenu_scope("Project"))
			{

			}
			//- Tools section
			if (const auto tools_scope = imgui::cmenu_scope("Tools"))
			{

			}

			//- Developer and debug section
			if (const auto dev_scope = imgui::cmenu_scope("Developer"))
			{
				imgui::cmenu_item("ImGui Demo", &ctx().m_imgui_demo);
				imgui::cmenu_item("ImPlot Demo", &ctx().m_implot_demo);
				imgui::cmenu_item("Docks", &ctx().m_docks_enabled);
			}

			if (const auto debug_scope = imgui::cmenu_scope("Debug"))
			{

			}
		}
		ImGui::EndMainMenuBar();

		//- Create a new project dialog. Might be useful to extend this to a 'create file' dialog or something...
		ui::cdialog dialog(C_CREATE_PROJECT_DIALOG_ID, &S_CREATE_PROJECT_WINDOW, ImGuiWindowFlags_None);

		if (S_CREATE_PROJECT_WINDOW && dialog
			.title("Create Project...")
			.icon(ICON_FA_DIAGRAM_PROJECT)
			.tooltip("Create Project...")
			.callback([]()
				{
					ui::ctext_input input("##inputProjectName");
					input
						.multiline(false)
						.value(&S_PROJECT_NAME_TEXT_BUFFER)
						.tooltip(S_PROJECT_NAME_TEXT_BUFFER)
						.option(ui::ctext_input::options_allow_tab, true)
						.hint("Project Name")
						.draw();

					ui::ctext_input folder("##projectFolderInputName");

					folder
						.multiline(false)
						.value(&S_PROJECT_PATH_TEXT_BUFFER)
						.tooltip(S_PROJECT_PATH_TEXT_BUFFER)
						.hint("Project Directory")
						.draw();

					ImGui::SameLine();

					if (ImGui::SmallButton(ICON_FA_FOLDER_OPEN))
					{
						S_PROJECT_PATH_TEXT_BUFFER = pfd::select_folder("Select Project Path", pfd::path::home()).result();
					}
				})
			.confirm_button("Confirm", []()
				{
				})
			.cancel_button("Cancel", []()
				{
				})
			.draw())
		{
			//- Verify specified data is correct
			auto verified = true;
			auto result = true;

			verified &= !S_PROJECT_NAME_TEXT_BUFFER.empty();
			verified &= !S_PROJECT_PATH_TEXT_BUFFER.empty();

			//- Create file if required
			if (verified && !core::cfilesystem::find_file(S_PROJECT_PATH_TEXT_BUFFER.data(), fmt::format("{}.project", S_PROJECT_NAME_TEXT_BUFFER.data()).data()) &&
				!core::cfilesystem::create_file_in(S_PROJECT_PATH_TEXT_BUFFER.data(), S_PROJECT_NAME_TEXT_BUFFER.data(), "project"))
			{
				result = false;
			}
			else if(verified)
			{
				editor::cproject::sconfig cfg;
				cfg.m_project_name = S_PROJECT_NAME_TEXT_BUFFER.data();
				cfg.m_basepath = S_PROJECT_PATH_TEXT_BUFFER.data();

				editor::cproject project(std::move(cfg));

				auto s = core::io::to_json_string(project);

				if (const auto save_result = core::fs::save_text_to_file(fmt::format("{}/{}.project", S_PROJECT_PATH_TEXT_BUFFER.data(), S_PROJECT_NAME_TEXT_BUFFER.data()), s.data());
					save_result != core::file_io_status_success)
				{
					result = false;
				}
			}

			if (result)
			{
				imgui::cui::create_notification("Success!", fmt::format("Successfully create project '{}' at path '{}'!", S_PROJECT_NAME_TEXT_BUFFER.data(), S_PROJECT_PATH_TEXT_BUFFER.data()),
					imgui::notification_type_success);

				S_PROJECT_NAME_TEXT_BUFFER.clear();
				S_PROJECT_PATH_TEXT_BUFFER.clear();
			}
			else
			{
				imgui::cui::create_notification("Failure!", fmt::format("Failed to create project '{}' at path '{}'!", S_PROJECT_NAME_TEXT_BUFFER.data(), S_PROJECT_PATH_TEXT_BUFFER.data()),
					imgui::notification_type_error);
			}
		}

		//- Showing common demo UIs
		if (ctx().m_implot_demo)
		{
			ImPlot::ShowDemoWindow(&ctx().m_implot_demo);
		}
		if (ctx().m_imgui_demo)
		{
			ImGui::ShowDemoWindow(&ctx().m_imgui_demo);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmain_menu::on_world_render()
	{
		m_elements_stack.on_world_render();
	}

} //- editor