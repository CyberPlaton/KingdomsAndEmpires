#include "entity_context_menu.hpp"

namespace editor
{
	namespace
	{
		constexpr std::string_view C_CONTEXT_MENU_POPUP_ID = "##entity_context_menu_popup";
		constexpr auto C_CONTEXT_MENU_POPUP_WINDOW_FLAGS = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration;
		constexpr auto C_CONTEXT_MENU_POPUP_FLAGS = ImGuiPopupFlags_None;

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	centity_context_menu::centity_context_menu(flecs::entity e, ccontext& ctx) :
		ccontext_holder(ctx), m_entity(e)
	{
		ImGui::OpenPopup(C_CONTEXT_MENU_POPUP_ID.data(), C_CONTEXT_MENU_POPUP_FLAGS);
	}

	//------------------------------------------------------------------------------------------------------------------------
	centity_context_menu::~centity_context_menu()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void centity_context_menu::on_ui_render()
	{
		if (ImGui::BeginPopup(C_CONTEXT_MENU_POPUP_ID.data(), C_CONTEXT_MENU_POPUP_WINDOW_FLAGS))
		{
			ImGui::Text("Option");
			ImGui::Text("Option");
			ImGui::Text("Option");
			ImGui::Text("Option");
			ImGui::Text("Option");
			ImGui::Text("Option");
			ImGui::Text("Option");
			ImGui::Text("Option");
			ImGui::Text("Option");

			ImGui::EndPopup();
		}
	}

} //- editor