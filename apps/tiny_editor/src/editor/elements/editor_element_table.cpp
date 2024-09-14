#include "editor_element_table.hpp"

namespace editor::ui
{
	//------------------------------------------------------------------------------------------------------------------------
	ctable::ctable(stringview_t id /*= {}*/, bool* enabled /*= nullptr*/, ImGuiTableFlags flags /*= ImGuiTableFlags_None*/) :
		m_id(id.data()), m_enabled(enabled ? *enabled : true), m_flags(flags), m_size(ImVec2(0, 0)), m_result(false)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctable& ctable::size(const vec2_t& value)
	{
		m_size = { value.x, value.y };
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctable& ctable::tooltip(stringview_t text, const bool show /*= true*/)
	{
		m_tooltip = text;
		m_show_tooltip = show;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctable& ctable::options(int op)
	{
		m_flags = op;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool ctable::draw()
	{
		{
			imgui::cdisabled_scope disabled_scope(!m_enabled);
		}

		if (m_show_tooltip)
		{
			imgui::cui::help_marker_no_question_mark(m_tooltip.data(), imgui::tooltip_hovering_delay_normal);
		}

		return m_result;
	}

} //- editor::ui