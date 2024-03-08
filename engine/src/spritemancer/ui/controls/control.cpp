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
				result = show_ui();
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

	} //- detail

} //- ui