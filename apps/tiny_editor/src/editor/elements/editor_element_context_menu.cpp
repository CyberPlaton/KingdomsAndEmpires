#include "editor_element_context_menu.hpp"

namespace editor::ui
{
	//------------------------------------------------------------------------------------------------------------------------
	ccontext_menu::~ccontext_menu()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool ccontext_menu::draw()
	{
		if (ImGui::BeginPopup(m_id, m_flags))
		{
			ImGui::EndPopup();
		}

		//- Returns false when the popup was closed
		return ImGui::IsPopupOpen(m_id, m_flags);
	}

	//------------------------------------------------------------------------------------------------------------------------
	ccontext_menu::ccontext_menu(const char* id, scontext& ctx, ImGuiWindowFlags flags /*= ImGuiWindowFlags_None*/) :
		m_id(id), m_ctx(ctx), m_flags(flags)
	{
	}

} //- editor::ui