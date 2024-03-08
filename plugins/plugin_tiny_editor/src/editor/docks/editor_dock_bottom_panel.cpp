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
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cbottom_panel::shutdown()
	{

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
		ImGui::End();



		ImGui::Begin("UI Test Window");

		ui::cbutton button;

		if (button.title("My Button")
			.icon(ICON_FA_APPLE_WHOLE)
			.tooltip("While using the archetype you might find yourself in a generational problem")
			.show())
		{
			sm::cui::create_notification("Notification", "My Button was pressed", sm::notification_type_info);
		}

		ImGui::End();
	}

} //- editor