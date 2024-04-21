#include "button.hpp"
#include "../sm_internal.hpp"

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
	ui::cbutton& cbutton::image(stringview_t _texture)
	{
		set_image(sm::ctx().tm().at(_texture));
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	detail::icontrol::click_result cbutton::show_ui()
	{
		scope::cid id_scope(imgui_id());
		auto result = detail::icontrol::click_result_none;

		if (algorithm::is_valid_handle(m_image))
		{
			const auto& tex = sm::ctx().tm().get(m_image);

			if (ImGui::ImageButton(m_title.data(), (ImTextureID)&tex.texture(), { SCAST(float, tex.width()), SCAST(float, tex.height()) }))
			{
				result = mousebutton_state();
			}
		}
		else
		{
			if (ImGui::SmallButton(m_icon.empty() ? m_title.data() : fmt::format("{} {}", m_icon, m_title).data()))
			{
				result = mousebutton_state();
			}
		}
		return result;
	}

} //- ui