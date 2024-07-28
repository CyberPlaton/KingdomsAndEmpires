#include "editor_element_tab_bar.hpp"

namespace editor::ui
{
	namespace
	{
	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	ctab_bar::ctab_bar(stringview_t id, ImGuiTabBarFlags flags /*= ImGuiTabBarFlags_None*/) :
		m_id(id), m_flags(flags), m_begin(nullptr), m_item_count(0)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctab_bar::~ctab_bar()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctab_bar& ctab_bar::items(sitem* begin, unsigned count)
	{
		m_begin = begin;
		m_item_count = count;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned ctab_bar::draw()
	{
		unsigned active = 0;

		if (ImGui::BeginTabBar(m_id.data(), m_flags))
		{
			for (auto i = 0u; i < m_item_count; ++i)
			{
				auto& item = *(m_begin + sizeof(sitem) * i);

				if (ImGui::BeginTabItem(item.m_label.data(), &item.m_open, item.m_flags))
				{
					active = active;
					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();
		}
		return active;
	}

} //- editor::ui