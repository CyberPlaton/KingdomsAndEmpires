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

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	bool cbottom_panel::init()
	{
		m_browser = std::make_shared<casset_browser>(ctx());
		return m_browser->init();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cbottom_panel::shutdown()
	{
		m_browser->shutdown();
		m_browser.reset();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cbottom_panel::on_ui_render()
	{
		if (!ctx().m_docks_enabled)
			return;

		const ImVec2 size = { algorithm::percent_value(C_WIDTH, ctx().window_width()),
			algorithm::percent_value(C_HEIGHT, ctx().window_height()) };
		const ImVec2 pos = { 0.0f, ctx().window_height() - algorithm::percent_value(C_HEIGHT, ctx().window_height()) + ctx().m_mainmenu_height };


		ImGui::SetNextWindowSize(size, ImGuiCond_Always);
		ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
		ImGui::Begin(C_BOTTOM_PANEL_ID.data(), nullptr, C_BOTTOM_PANEL_FLAGS);
		m_browser->on_ui_render();
		ImGui::End();



		ImGui::SetNextWindowPos({ (float)1920 / 2.0f, (float)1080 / 2.0f }, ImGuiCond_Appearing);
		ImGui::Begin("Camera");

		if (ecs::cworld_manager::instance().has_active())
		{
			const auto& w = ecs::cworld_manager::instance().active();
		}

		ImGui::End();
	}

} //- editor