#pragma once
#include "imgui/rlImGui.h"
#include "imgui/imgui.h"
#include "imgui/imgui-knobs.h"
#include "imgui/implot.h"
#include "imgui/imgui_node_editor.h"

namespace ui
{
	ImGuiID generate_id(const std::string& title, const std::string& icon = "");
	ImVec4 convert_to_imgui_color(const core::scolor& color);

} //- ImGui