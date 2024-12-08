#include "editor_main_menu_dock.hpp"
#include "../elements/editor_element_context_menu.hpp"
#include "material_editor/editor_material_editor.hpp"
#include "text_editor/editor_text_editor.hpp"
#include <pfd.h>

namespace editor
{
	namespace
	{
		constexpr stringview_t C_CREATE_PROJECT_DIALOG_ID = "##createProjectDialog";
		constexpr stringview_t C_CREATE_WORLD_DIALOG_ID = "##createWorldDialog";
		constexpr stringview_t C_PROJECT_SETTINGS_WINDOW_ID = "##projectSettingsWindow";
		//- Note: when adding more strings for text input, dont forget to resize them
		static string_t S_PROJECT_NAME_TEXT_BUFFER;
		static string_t S_PROJECT_PATH_TEXT_BUFFER;
		static string_t S_WORLD_NAME_TEXT_BUFFER;
		static bool S_CREATE_PROJECT_WINDOW = false;
		static bool S_CREATE_WORLD_WINDOW = false;
		static bool S_PROJECT_SETTINGS_WINDOW = false;
		static const char* C_PROJECT_EXTENSION[] = {".project"};
		static vector_t<string_t> S_WORLD_MODULES;
		static ecs::world::sconfig S_WORLD_CONFIG;

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	bool cmain_menu::init()
	{
		S_PROJECT_NAME_TEXT_BUFFER.resize(128);
		S_PROJECT_PATH_TEXT_BUFFER.resize(256);
		S_WORLD_NAME_TEXT_BUFFER.resize(128);
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmain_menu::shutdown()
	{
		m_elements_stack.shutdown();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmain_menu::on_update(float dt)
	{
		m_elements_stack.on_update(dt);
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
					imgui::cmenu_item("Project", &S_CREATE_PROJECT_WINDOW);
					imgui::cmenu_item("World", &S_CREATE_WORLD_WINDOW);
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
				imgui::cmenu_item("Settings", &S_PROJECT_SETTINGS_WINDOW);
			}
			//- Tools section
			if (const auto tools_scope = imgui::cmenu_scope("Tools"))
			{
				if (imgui::cmenu_item("Material Editor"))
				{
					m_elements_stack.push<cmaterial_editor>(ctx());
				}
				if (imgui::cmenu_item("Text Editor"))
				{
					m_elements_stack.push<textedit::ctext_editor>(ctx());
				}
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

		show_create_project_dialog(&S_CREATE_PROJECT_WINDOW);
		show_create_world_dialog(&S_CREATE_WORLD_WINDOW);
		show_project_settings(&S_PROJECT_SETTINGS_WINDOW);

		//- Showing common demo UIs
		if (ctx().m_implot_demo)
		{
			ImPlot::ShowDemoWindow(&ctx().m_implot_demo);
		}
		if (ctx().m_imgui_demo)
		{
			ImGui::ShowDemoWindow(&ctx().m_imgui_demo);
		}

		m_elements_stack.on_ui_render();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmain_menu::on_world_render()
	{
		m_elements_stack.on_world_render();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmain_menu::show_create_project_dialog(bool* open)
	{
		//- Create a new project dialog. Might be useful to extend this to a 'create file' dialog or something...
		ui::cdialog dialog(open, C_CREATE_PROJECT_DIALOG_ID, ImGuiWindowFlags_None);

		if (*open && dialog
			.title("Create Project...")
			.icon(ICON_FA_ADDRESS_BOOK)
			.tooltip("Create Project...")
			.callback([&]()
				{
					ui::ctext_input input("##inputProjectName");
					input
						.multiline(false)
						.resizeable(true)
						.value(&S_PROJECT_NAME_TEXT_BUFFER)
						.tooltip(S_PROJECT_NAME_TEXT_BUFFER)
						.options(ui::ctext_input::options_allow_tab)
						.hint("Project Name")
						.draw();

					ui::ctext_input folder("##projectFolderInputName");

					folder
						.multiline(false)
						.resizeable(true)
						.value(&S_PROJECT_PATH_TEXT_BUFFER)
						.tooltip(S_PROJECT_PATH_TEXT_BUFFER)
						.hint("Project Directory")
						.draw();

					ImGui::SameLine();

					if (ImGui::SmallButton(ICON_FA_ADDRESS_BOOK))
					{
						S_PROJECT_PATH_TEXT_BUFFER = pfd::select_folder("Select Project Path", pfd::path::home()).result();
					}
				})
			.draw())
		{
			auto verified = true;
			auto result = false;

			verified &= !S_PROJECT_NAME_TEXT_BUFFER.empty();
			verified &= !S_PROJECT_PATH_TEXT_BUFFER.empty();

			if (verified)
			{
				result = engine::cengine::service<engine::cproject_service>()->create_project(S_PROJECT_PATH_TEXT_BUFFER, S_PROJECT_NAME_TEXT_BUFFER) != nullptr;
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
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmain_menu::show_create_world_dialog(bool* open)
	{
		ui::cdialog dialog(open, C_CREATE_WORLD_DIALOG_ID, ImGuiWindowFlags_None);

		if (*open && dialog
			.title("Create World...")
			.icon(ICON_FA_ADDRESS_BOOK)
			.tooltip("Create World...")
			.callback([&]()
				{
					//- Create and modify world configuration
					ui::ctext_input input("##inputWorldName");
					input
						.multiline(false)
						.resizeable(true)
						.value(&S_WORLD_NAME_TEXT_BUFFER)
						.tooltip(S_WORLD_NAME_TEXT_BUFFER)
						.options(ui::ctext_input::options_allow_tab)
						.hint("World Name")
						.draw();

					if (ImGui::CollapsingHeader("Modules"))
					{
						for (auto i = 0; i < S_WORLD_CONFIG.m_modules.size(); ++i)
						{
							ui::ctext_input input(fmt::format("##inputModule_{}", i));
							input
								.multiline(false)
								.resizeable(true)
								.value(&S_WORLD_CONFIG.m_modules[i])
								.tooltip(S_WORLD_CONFIG.m_modules[i])
								.options(ui::ctext_input::options_true_on_enter)
								.hint("Module Name")
								.draw();
						}

						if (ImGui::SmallButton(ICON_FA_PLUS))
						{
							S_WORLD_CONFIG.m_modules.emplace_back();
						}
					}

					imgui::cui::edit_field_uint32(S_WORLD_CONFIG.m_threads, 1, engine::cthread_service::hardware_threads() / 2,
						"World Threads", "The number of threads the world will be using. The maximum count orients itself on currents PC hardware threads!", 10000, 10001);
				})
			.draw())
		{
			auto result = false;

			//- Verify that world name is not empty
			string_t name(S_WORLD_NAME_TEXT_BUFFER.data());

			if (!name.empty())
			{
				S_WORLD_CONFIG.m_name = name;
				S_WORLD_CONFIG.m_threads = 1;

				result = ecs::cworld_manager::instance().create(S_WORLD_CONFIG, !ecs::cworld_manager::instance().has_active());
			}

			if (result)
			{
				imgui::cui::create_notification("Success!", fmt::format("Successfully created world '{}'!", S_WORLD_NAME_TEXT_BUFFER.data()),
					imgui::notification_type_success);

				S_WORLD_NAME_TEXT_BUFFER.clear();
				*open = false;
				S_WORLD_CONFIG = ecs::world::sconfig();
			}
			else
			{
				imgui::cui::create_notification("Failure!", fmt::format("Failed to create world '{}'!", S_WORLD_NAME_TEXT_BUFFER.data()),
					imgui::notification_type_error);
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmain_menu::show_project_settings(bool* open)
	{

	}

} //- editor