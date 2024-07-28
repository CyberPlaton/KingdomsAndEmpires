#include "editor_dock_bottom_panel.hpp"

namespace editor
{
	namespace
	{
		constexpr unsigned C_WIDTH = 100;
		constexpr unsigned C_HEIGHT = 30;
		constexpr std::string_view C_BOTTOM_PANEL_ID = "##editor_dock_bottom_panel";
		constexpr auto C_BOTTOM_PANEL_FLAGS = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

		constexpr std::string_view C_TAB_BAR_ID = "##editor_dock_bottom_panel_tab_bar";
		constexpr auto C_TAB_BAR_FLAGS = ImGuiTabBarFlags_None;

		static ImVec2 S_SIZE = { 0, 0 };
		static ImVec2 S_POS = { 0, 0 };

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	bool cbottom_panel::init()
	{
		if (auto browser = m_elements_stack.push<casset_browser>(ctx()); browser && browser->init())
		{
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cbottom_panel::shutdown()
	{
		m_elements_stack.shutdown();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cbottom_panel::on_ui_render()
	{
		if (!ctx().m_docks_enabled)
			return;

		ImGui::SetNextWindowSize(S_SIZE, ImGuiCond_Always);
		ImGui::SetNextWindowPos(S_POS, ImGuiCond_Always);
		ImGui::Begin(C_BOTTOM_PANEL_ID.data(), nullptr, C_BOTTOM_PANEL_FLAGS);

		//- Display tab
		auto active = ui::ctab_bar(C_TAB_BAR_ID, C_TAB_BAR_FLAGS)
			.items(m_tab_bar_items.data(), (unsigned)m_tab_bar_items.size())
			.draw();

		//- Display all other elements
		m_elements_stack.on_ui_render();

		//- TODO: make it possible to run a specific element from stack?!
		//m_elements[active]->on_ui_render();

		ImGui::End();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cbottom_panel::on_world_render()
	{
		m_elements_stack.on_world_render();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cbottom_panel::on_update(float dt)
	{
		S_SIZE = { algorithm::percent_value(C_WIDTH, ctx().m_window_width), algorithm::percent_value(C_HEIGHT, ctx().m_window_height) };
		S_POS = { 0.0f, ctx().m_window_height - algorithm::percent_value(C_HEIGHT, ctx().m_window_height) + ctx().m_mainmenu_height };

		m_elements_stack.on_update(dt);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cbottom_panel::on_post_update(float dt)
	{
		m_elements_stack.on_post_update(dt);
	}

} //- editor