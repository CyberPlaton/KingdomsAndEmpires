#pragma once
#include <core.h>
#include <imgui.h>
#include "imgui/rlImGui.h"

namespace ImNodes = ax::NodeEditor;

namespace imgui
{
	namespace detail
	{
		static ImPlotContext*			S_IMPLOT_CONTEXT = nullptr;
		static ImNodes::EditorContext*	S_IMNODES_CONTEXT = nullptr;
		static ImGuiContext*			S_IMGUI_CONTEXT = nullptr;

	} //- detail

	bool init();
	void shutdown();
	void begin();
	void end();

	//------------------------------------------------------------------------------------------------------------------------
	static ImPlotContext* implot_ctx()
	{
		if (!detail::S_IMPLOT_CONTEXT)
		{
			detail::S_IMPLOT_CONTEXT = ImPlot::GetCurrentContext();
		}
		return detail::S_IMPLOT_CONTEXT;
	}

	//------------------------------------------------------------------------------------------------------------------------
	static ImNodes::EditorContext* imnodes_ctx()
	{
		if (!detail::S_IMNODES_CONTEXT)
		{
			detail::S_IMNODES_CONTEXT = ImNodes::GetCurrentEditor();
		}
		return detail::S_IMNODES_CONTEXT;
	}

	//------------------------------------------------------------------------------------------------------------------------
	static ImGuiContext* imgui_ctx()
	{
		if (!detail::S_IMGUI_CONTEXT)
		{
			detail::S_IMGUI_CONTEXT = ImGui::GetCurrentContext();
		}
		return detail::S_IMGUI_CONTEXT;
	}

} //- imgui

namespace ImGui
{
	bool BufferingBar(const char* label, float value, const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col);
	bool Spinner(const char* label, float radius, int thickness, const ImU32& color);

} //- ImGui