#include "imgui_integration.hpp"

namespace ui
{
	//------------------------------------------------------------------------------------------------------------------------
	ImGuiID generate_id(const std::string& title, const std::string& icon /*= ""*/)
	{
		return ImGui::GetID(fmt::format("##{}{}", icon, title).c_str());
	}

	//------------------------------------------------------------------------------------------------------------------------
	ImVec4 convert_to_imgui_color(const core::scolor& color)
	{
		auto out = color.normalize();
		return {out.r, out.g, out.b, out.a};
	}

} //- ui