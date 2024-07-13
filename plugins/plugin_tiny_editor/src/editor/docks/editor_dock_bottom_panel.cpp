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

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	bool cbottom_panel::init()
	{
		auto browser = detail::create_ui_element<casset_browser>(ctx());

		if (browser->init())
		{
			m_elements.push_back(browser);

			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cbottom_panel::shutdown()
	{
		for (const auto& element : m_elements)
		{
			element->shutdown();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cbottom_panel::on_ui_render()
	{
		if (!ctx().m_docks_enabled)
			return;

		const ImVec2 size = { algorithm::percent_value(C_WIDTH, ctx().m_window_width),
			algorithm::percent_value(C_HEIGHT, ctx().m_window_height) };
		const ImVec2 pos = { 0.0f, ctx().m_window_height - algorithm::percent_value(C_HEIGHT, ctx().m_window_height) + ctx().m_mainmenu_height };


		ImGui::SetNextWindowSize(size, ImGuiCond_Always);
		ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
		ImGui::Begin(C_BOTTOM_PANEL_ID.data(), nullptr, C_BOTTOM_PANEL_FLAGS);

		auto active = ui::ctab_bar(C_TAB_BAR_ID, C_TAB_BAR_FLAGS)
						.items(m_tab_bar_items.data(), (unsigned)m_tab_bar_items.size())
						.draw();

		m_elements[active]->on_ui_render();

		ImGui::End();
	}

} //- editor