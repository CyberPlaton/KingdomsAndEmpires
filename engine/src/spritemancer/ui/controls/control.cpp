#include "control.hpp"
#include "../../sm_internal.hpp"

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
			m_active(true), m_image(nullptr)
		{
		}

		//------------------------------------------------------------------------------------------------------------------------
		icontrol::click_result icontrol::show()
		{
			auto result = click_result_none;
			if(m_active)
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

		//------------------------------------------------------------------------------------------------------------------------
		ImGuiID icontrol::imgui_id() const
		{
			return m_id.empty() ? ui::generate_id(m_title, m_icon) : ui::generate_id(m_id);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void icontrol::set_image(stringview_t name)
		{
			m_image = sm::ctx().tm().native(name);
		}

		//------------------------------------------------------------------------------------------------------------------------
		icontrol::click_result icontrol::mousebutton_state()
		{
			auto result = click_result_none;
			if (ImGui::IsMouseDown(click_result_lmb))
			{
				result = click_result_lmb;
			}
			else if (ImGui::IsMouseDown(click_result_rmb))
			{
				result = click_result_rmb;
			}
			else if (ImGui::IsMouseDown(click_result_mmb))
			{
				result = click_result_mmb;
			}
			return result;
		}

	} //- detail

} //- ui