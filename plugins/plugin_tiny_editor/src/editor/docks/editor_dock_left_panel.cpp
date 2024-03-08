#include "editor_dock_left_panel.hpp"

namespace editor
{
	namespace
	{
		constexpr unsigned C_WIDTH = 25;
		constexpr unsigned C_HEIGHT = 70;
		constexpr std::string_view C_LEFT_PANEL_ID = "##editor_dock_left_panel";
		constexpr auto C_LEFT_PANEL_FLAGS = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	bool cleft_panel::init()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cleft_panel::shutdown()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cleft_panel::on_ui_render()
	{
		if (!ctx().m_docks_enabled)
			return;

		const ImVec2 size = {algorithm::percent_value(C_WIDTH, ctx().window_width()),
			algorithm::percent_value(C_HEIGHT, ctx().window_height()) };
		const ImVec2 pos = {0.0f, ctx().m_mainmenu_height};


		ImGui::SetNextWindowSize(size, ImGuiCond_Always);
		ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
		ImGui::Begin(C_LEFT_PANEL_ID.data(), nullptr, C_LEFT_PANEL_FLAGS);
		ImGui::End();
	}

} //- editor