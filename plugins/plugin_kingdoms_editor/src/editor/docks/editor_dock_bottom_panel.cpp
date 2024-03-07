#include "editor_dock_bottom_panel.hpp"

namespace editor
{
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
		ImGui::Begin("UI Test Window");

		ui::cbutton button;

		if (button.title("My Button")
			.icon(ICON_FA_APPLE_WHOLE)
			.tooltip("While using the archetype you might find yourself in a generational problem")
			.color(ImGuiCol_Text, core::scolor(core::common_color_pink50))
			.color(ImGuiCol_Button, core::scolor(core::common_color_pink400))
			.show())
		{
			sm::cui::create_notification("Notification", "My Button was pressed", sm::notification_type_info);
		}

		ImGui::End();
	}

} //- editor