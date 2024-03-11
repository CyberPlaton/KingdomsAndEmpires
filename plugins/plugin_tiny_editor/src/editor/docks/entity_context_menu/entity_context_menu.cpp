#include "entity_context_menu.hpp"

namespace editor
{
	namespace
	{
		constexpr std::string_view C_CONTEXT_MENU_POPUP_ID = "##entity_context_menu";
		constexpr auto C_CONTEXT_MENU_WINDOW_FLAGS = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration;
		constexpr auto C_CONTEXT_MENU_POPUP_FLAGS = ImGuiPopupFlags_None;

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	centity_context_menu::centity_context_menu(ccontext& ctx) :
		ccontext_holder(ctx)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	centity_context_menu::~centity_context_menu()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void centity_context_menu::on_ui_render()
	{
		if (m_open)
		{
			//- 'opening' a popup should be done once
			ImGui::OpenPopup(C_CONTEXT_MENU_POPUP_ID.data(), C_CONTEXT_MENU_POPUP_FLAGS);
			m_open = false;
		}

		if (ImGui::BeginPopup(C_CONTEXT_MENU_POPUP_ID.data(), C_CONTEXT_MENU_WINDOW_FLAGS))
		{
			ImGui::TextUnformatted("Hi");
			ImGui::TextUnformatted("Hi");
			ImGui::TextUnformatted("Hi");
			ImGui::TextUnformatted("Hi");

			ImGui::EndPopup();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void centity_context_menu::open(flecs::entity e)
	{
		m_entity = e;
		m_open = true;
	}

} //- editor