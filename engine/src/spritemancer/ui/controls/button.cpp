#include "button.hpp"

namespace ui
{
	namespace detail
	{
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
		scope::cid id_scope(imgui_id());

		const auto result = ImGui::SmallButton(m_icon.empty() ? m_title.data() : fmt::format("{} {}", m_icon, m_title).data());
		return result;
	}

} //- ui