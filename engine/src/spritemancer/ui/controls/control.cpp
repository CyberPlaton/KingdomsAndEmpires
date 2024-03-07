#include "control.hpp"

namespace ui
{
	namespace scope
	{
		//------------------------------------------------------------------------------------------------------------------------
		cwrap_text::cwrap_text(size_t text_length)
		{
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * text_length);
		}

		//------------------------------------------------------------------------------------------------------------------------
		cwrap_text::cwrap_text(const std::string& text)
		{
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * text.length());
		}

		//------------------------------------------------------------------------------------------------------------------------
		cwrap_text::~cwrap_text()
		{
			ImGui::PopTextWrapPos();
		}

		//------------------------------------------------------------------------------------------------------------------------
		cdisabled::cdisabled()
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
		}

		//------------------------------------------------------------------------------------------------------------------------
		cdisabled::~cdisabled()
		{
			ImGui::PopStyleColor();
		}

		//------------------------------------------------------------------------------------------------------------------------
		cstyle_color::cstyle_color(ImGuiCol var, core::scolor color)
		{
			ImGui::PushStyleColor(var, convert_to_imgui_color(color));
		}

		//------------------------------------------------------------------------------------------------------------------------
		cstyle_color::~cstyle_color()
		{
			ImGui::PopStyleColor();
		}

		//------------------------------------------------------------------------------------------------------------------------
		cid::cid(ImGuiID id)
		{
			ImGui::PushID(id);
		}

		//------------------------------------------------------------------------------------------------------------------------
		cid::~cid()
		{
			ImGui::PopID();
		}

	} //- scope

	namespace detail
	{
		//------------------------------------------------------------------------------------------------------------------------
		icontrol::icontrol() :
			m_active(true)
		{
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool icontrol::show()
		{
			auto result = false;
			{
				for (const auto& colors : m_colors)
				{
					ImGui::PushStyleColor(colors.m_variable, colors.m_color);
				}
				result = show_ui();

				ImGui::PopStyleColor(m_colors.size());
			}
			show_tooltip();
			return result;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void icontrol::show_tooltip()
		{
			if (!m_tooltip.empty() && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
			{
				ImGui::BeginTooltip();
				scope::cwrap_text wrap(m_tooltip.length());
				ImGui::TextUnformatted(m_tooltip.data());
				ImGui::EndTooltip();
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cui_element::color(ImGuiCol var, core::scolor _color)
		{
			m_colors.emplace_back(scoloring{ var, ui::convert_to_imgui_color(_color) });
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cui_element::style(ImGuiStyleVar var, float value)
		{
			auto& styling = m_styles.emplace_back();
			styling.m_type = sstyling::variable_type_float;
			styling.m_value_float = value;
			styling.m_variable = var;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cui_element::style(ImGuiStyleVar var, vec2_t value)
		{
			auto& styling = m_styles.emplace_back();
			styling.m_type = sstyling::variable_type_vec2;
			styling.m_value_vec2 = { value.x, value.y };
			styling.m_variable = var;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cui_element::push_color_vars()
		{
			for (const auto& colors : m_colors)
			{
				ImGui::PushStyleColor(colors.m_variable, colors.m_color);
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cui_element::push_style_vars()
		{
			for (const auto& style : m_styles)
			{
				switch (style.m_type)
				{
				case sstyling::variable_type_float:
				{
					ImGui::PushStyleVar(style.m_variable, style.m_value_float);
					break;
				}
				case sstyling::variable_type_vec2:
				{
					ImGui::PushStyleVar(style.m_variable, style.m_value_vec2);
					break;
				}
				default:
					break;
				}
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cui_element::pop_color_vars()
		{
			ImGui::PopStyleColor(m_colors.size());
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cui_element::pop_style_vars()
		{
			ImGui::PopStyleVar(m_styles.size());
		}

	} //- detail

	//------------------------------------------------------------------------------------------------------------------------
	ccontrol& ccontrol::color(ImGuiCol var, core::scolor _color)
	{
		m_colors.emplace_back(scoloring{ var, ui::convert_to_imgui_color(_color) });
		return *this;
	}

} //- ui