#include "button.hpp"

namespace ui
{
	namespace detail
	{
		//------------------------------------------------------------------------------------------------------------------------
		ibutton::ibutton() :
			m_active(true)
		{
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool ibutton::show()
		{
			auto result = show_ui();
			show_tooltip();
			return result;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ibutton::show_tooltip()
		{
			if (!m_tooltip.empty() && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
			{
				ImGui::BeginTooltip();
				{
					scope::cwrap_text wrap(m_tooltip.length());
					ImGui::TextUnformatted(m_tooltip.c_str());
				}
				ImGui::EndTooltip();
			}
		}

	} //- detail

	//------------------------------------------------------------------------------------------------------------------------
	ui::cbutton& cbutton::id(stringview_t _id)
	{
		m_id = _id;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ui::cbutton& cbutton::title(stringview_t _title)
	{
		m_title = _title;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ui::cbutton& cbutton::tooltip(stringview_t _tooltip)
	{
		m_tooltip = _tooltip;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ui::cbutton& cbutton::icon(stringview_t _icon)
	{
		m_icon = _icon;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cbutton::show_ui()
	{
		const auto id = m_id.empty() ? ui::generate_id(m_title, m_icon) : ui::generate_id(m_id);

		scope::cid id_scope(id);

		const auto result = ImGui::SmallButton(m_icon.empty() ? m_title.data() : fmt::format("{} {}", m_icon, m_title).data());
		return result;
	}

} //- ui