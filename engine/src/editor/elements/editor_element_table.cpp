#include "editor_element_table.hpp"

namespace editor::ui
{
	//------------------------------------------------------------------------------------------------------------------------
	ctable::ctable(stringview_t id, bool* enabled /*= nullptr*/, ImGuiTableFlags flags /*= ImGuiTableFlags_None*/) :
		m_id(id.data()), m_enabled(enabled ? *enabled : true), m_flags(flags), m_size(ImVec2(0, 0)), m_result(false),
		m_show_tooltip(false)
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
	bool ctable::begin(const unsigned columns, const vector_t<table_column_data_t>& data /*= {}*/)
	{
		m_columns = columns;

		if (m_size.x == 0.0f || m_size.y == 0.0f)
		{
			const auto available = ImGui::GetContentRegionAvail();
			m_size.x = available.x;
			m_size.y = available.y;
		}

		imgui::cdisabled_scope disabled_scope(!m_enabled);
		m_result = ImGui::BeginTable(m_id.data(), m_columns, m_flags, m_size);

		if (!data.empty())
		{
			for (auto i = 0; i < data.size(); ++i)
			{
				const auto& [name, flag, weight] = data[i];
				ImGui::TableSetupColumn(name.data(), flag, weight);
			}
		}

		ImGui::TableHeadersRow();

		return m_result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctable& ctable::begin_next_row()
	{
		ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctable& ctable::begin_column(const unsigned idx)
	{
		ImGui::TableSetColumnIndex(idx);
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ctable::end()
	{
		if (m_result)
		{
			ImGui::EndTable();
		}

		if (m_show_tooltip)
		{
			imgui::cui::help_marker_no_question_mark(m_tooltip.data(), imgui::tooltip_hovering_delay_normal);
		}

		//- Reset dynamic state for next tick if table is used in retained mode
		m_size = ImVec2(0, 0);
		m_result = false;
		m_show_tooltip = false;
		m_columns = 0;
	}

} //- editor::ui