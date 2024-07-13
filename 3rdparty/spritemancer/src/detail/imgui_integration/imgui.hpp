#pragma once
#include <core.h>
#include "imgui/rlImGui.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/implot.h"
#include "imgui/imnodes.h"
#include "imgui/imnodes_internal.h"
#include "imgui/IconsFontAwesome6.h"
#include "imgui/TextEditor.h"

namespace imgui
{
	bool init();
	void shutdown();
	void begin();
	void end();

} //- imgui

namespace ImGui
{
	bool BufferingBar(const char* label, float value, const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col);
	bool Spinner(const char* label, float radius, int thickness, const ImU32& color);

} //- ImGui