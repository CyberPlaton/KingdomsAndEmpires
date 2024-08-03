#include "editor_main_menu_dock.hpp"
#include "../elements/editor_element_context_menu.hpp"

namespace editor
{
	namespace
	{
		static bool S_CREATE_PROJECT_WINDOW = false;
		static const char* C_PROJECT_EXTENSION[] = {".project"};

		//------------------------------------------------------------------------------------------------------------------------
		class cmenu_item final : core::cnon_copyable
		{
		public:
			cmenu_item(const char* name, bool* selected = nullptr, const bool enabled = true);
			~cmenu_item() = default;

			operator bool() const noexcept { return m_result; }

		private:
			bool m_result = false;
		};

		//------------------------------------------------------------------------------------------------------------------------
		cmenu_item::cmenu_item(const char* name, bool* selected /*= nullptr*/, const bool enabled /*= true*/)
		{
			m_result = ImGui::MenuItem(name, nullptr, selected, enabled);
		}

		//------------------------------------------------------------------------------------------------------------------------
		class cmenu_scope final : core::cnon_copyable
		{
		public:
			cmenu_scope(const char* name, const bool enabled = true);
			~cmenu_scope();

			operator bool() const noexcept{ return m_result; }

		private:
			bool m_result = false;
		};

		//------------------------------------------------------------------------------------------------------------------------
		cmenu_scope::cmenu_scope(const char* name, const bool enabled /*= true*/)
		{
			m_result = ImGui::BeginMenu(name, enabled);
		}

		//------------------------------------------------------------------------------------------------------------------------
		cmenu_scope::~cmenu_scope()
		{
			if (m_result) { ImGui::EndMenu(); }
		}


	} //- unnamed

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

			//- TODO: the editor main menu or a part of it should be extendable from any part of editor, i.e.
			//- a plugin would like to offer his own menu to set up settings etc., we want to allow this
			//- in an orderly fashion. However, for now we dont do that.

			//- General editor section
			if (const auto editor_scope = cmenu_scope("Editor"))
			{
				if (const auto new_scope = cmenu_scope("New"))
				{
					cmenu_item("Project", &S_CREATE_PROJECT_WINDOW, true);
				}
				if (const auto pref_cope = cmenu_scope("Preferences"))
				{

				}
				if (const auto exit_scope = cmenu_scope("Exit"))
				{

				}
			}
			//- Project specific section
			if (const auto project_scope = cmenu_scope("Project"))
			{

			}
			//- Tools section
			if (const auto tools_scope = cmenu_scope("Tools"))
			{

			}

			//- Developer and debug section
			if (const auto dev_scope = cmenu_scope("Developer"))
			{
				cmenu_item("Docks", &ctx().m_docks_enabled);
			}

			if (const auto debug_scope = cmenu_scope("Debug"))
			{

			}
		}
		ImGui::EndMainMenuBar();

		if (S_CREATE_PROJECT_WINDOW)
		{
			auto mouse = ImGui::GetMousePos();
			auto dir = core::cfilesystem::cwd();

			imgui::cui::create_file_dialog("Create Project...", "", &S_CREATE_PROJECT_WINDOW, {mouse.x, mouse.y}, {250, 200}, C_PROJECT_EXTENSION, 1, dir.view());
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmain_menu::on_world_render()
	{
		m_elements_stack.on_world_render();
	}

} //- editor