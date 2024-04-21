#include "tree_node.hpp"
#include "../sm_internal.hpp"

namespace ui
{
	//------------------------------------------------------------------------------------------------------------------------
	ui::ctreenode& ctreenode::id(stringview_t _id)
	{
		m_id = _id;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ui::ctreenode& ctreenode::title(stringview_t _title)
	{
		m_title = _title;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ui::ctreenode& ctreenode::tooltip(stringview_t _tooltip)
	{
		m_tooltip = _tooltip;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ui::ctreenode& ctreenode::flags(ImGuiTreeNodeFlags _flags)
	{
		m_flags = _flags;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ui::ctreenode& ctreenode::icon(stringview_t _icon)
	{
		m_icon = _icon;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ui::ctreenode& ctreenode::image(stringview_t _texture)
	{
		m_image = sm::ctx().tm().at(_texture);
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	detail::icontrol::click_result ctreenode::show_ui()
	{
		scope::cid id_scope(imgui_id());
		auto result = detail::icontrol::click_result_none;

		if (algorithm::is_valid_handle(m_image))
		{
			const auto& tex = sm::ctx().tm().get(m_image);

			ImGui::Image((ImTextureID)&tex.texture(), {SCAST(float, tex.width()), SCAST(float, tex.height())});
			ImGui::SameLine(0.0f, 0.0f);
		}
		if (ImGui::TreeNodeEx(m_icon.empty() ? m_title.data() : fmt::format("{} {}", m_icon, m_title).data(), m_flags))
		{
			result = mousebutton_state();
			ImGui::TreePop();
		}
		return result;
	}

} //- ui