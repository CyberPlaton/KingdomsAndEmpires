#include "editor_element_context_menu.hpp"

namespace editor::ui
{
	namespace
	{
		constexpr ImVec2 C_CONTEXT_MENU_SIZE_MIN = { 200.0f, FLT_MAX };

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	ccontext_menu::ccontext_menu(stringview_t id, scontext& ctx, ImGuiWindowFlags flags /*= ImGuiWindowFlags_None*/) :
		m_id(id), m_ctx(ctx), m_flags(flags | ImGuiWindowFlags_MenuBar)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	ccontext_menu::~ccontext_menu()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccontext_menu::draw()
	{
		CORE_NAMED_ZONE(ccontext_menu::draw);

		ImGui::SetNextWindowSize(C_CONTEXT_MENU_SIZE_MIN, ImGuiCond_Appearing);

		if (ImGui::BeginPopup(m_id.data(), m_flags))
		{
			//- Show context menu
			show_submenu(m_entries);

			ImGui::EndPopup();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	ccontext_menu& ccontext_menu::begin(stringview_t label, const bool enabled /*= true*/)
	{
		auto menu = std::make_shared<sentry>();

		menu->m_parent = m_current;
		menu->m_label = label;
		menu->m_enabled = enabled;
		menu->m_type = entry_type_menu;

		//- Set root entry if none present
		if (!m_current)
		{
			m_entries.push_back(menu);
		}
		else
		{
			m_current->m_entries.push_back(menu);
		}

		m_current = menu;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ccontext_menu& ccontext_menu::end()
	{
		CORE_ASSERT(m_current, "Invalid operation. Begin/end mismatch!");

		m_current = m_current->m_parent;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ccontext_menu& ccontext_menu::item(stringview_t label, bool* selected, item_callback_t&& callback, const bool enabled /*= true*/)
	{
		CORE_ASSERT(m_current, "Invalid operation. No menu specified for entry!");

		auto entry = std::make_shared<sentry>();

		entry->m_parent = m_current;
		entry->m_callback = std::move(callback);
		entry->m_label = label;
		entry->m_enabled = enabled;
		entry->m_selected = selected;
		entry->m_type = entry_type_item;

		m_current->m_entries.push_back(entry);

		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccontext_menu::show_submenu(const vector_t<ref_t<sentry>>& entries)
	{
		for (const auto& entry : entries)
		{
			switch (entry->m_type)
			{
			case entry_type_menu:
			{
				if (ImGui::BeginMenu(entry->m_label.data(), entry->m_enabled))
				{
					if (!entry->m_entries.empty())
					{
						show_submenu(entry->m_entries);
					}

					ImGui::EndMenu();
				}
				break;
			}
			case entry_type_item:
			{
				if (ImGui::MenuItem(entry->m_label.data(), nullptr, entry->m_selected, entry->m_enabled))
				{
					entry->m_callback();
				}
				break;
			}
			case entry_type_none:
			default:
				break;
			}
		}
	}

} //- editor::ui