#include "imgui.hpp"

namespace imgui
{
	namespace
	{
		static auto S_W = 0;
		static auto S_H = 0;

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	bool init()
	{
		//- init imgui and create icon font from ICON_FA data
		detail::S_IMGUI_CONTEXT = ImGui::CreateContext(nullptr);
		detail::S_IMNODES_CONTEXT = ImNodes::CreateEditor(nullptr);
		detail::S_IMPLOT_CONTEXT = ImPlot::CreateContext();
		rlImGuiSetup(true);

		//- setup default style
		ImGui::GetStyle().WindowRounding = 0;
		ImGui::GetStyle().ChildRounding = 12;
		ImGui::GetStyle().FrameRounding = 6;
		ImGui::GetStyle().PopupRounding = 12;
		ImGui::GetStyle().ScrollbarRounding = 9;
		ImGui::GetStyle().GrabRounding = 12;
		ImGui::GetStyle().TabRounding = 6;

		auto& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

		//- setup resize handler
		core::cservice_manager::find<core::cevent_service>()->emplace_listener<events::window::sresize>([](const rttr::variant& var)
			{
				const auto& e = var.convert<events::window::sresize>();
				S_W = e.w;
				S_H = e.h;
			});

		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void shutdown()
	{
		ImPlot::DestroyContext(detail::S_IMPLOT_CONTEXT);
		ImNodes::DestroyEditor(detail::S_IMNODES_CONTEXT);
		rlImGuiShutdown();
		ImGui::DestroyContext(detail::S_IMGUI_CONTEXT);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void begin()
	{
		rlImGuiBegin();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void end()
	{
		rlImGuiEnd();
	}

} //- imgui

namespace ImGui
{
	//- reference: https://github.com/ocornut/imgui/issues/1901
	//------------------------------------------------------------------------------------------------------------------------
	bool BufferingBar(const char* label, float value, const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = size_arg;
		size.x -= style.FramePadding.x * 2;

		const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
		ItemSize(bb, style.FramePadding.y);
		if (!ItemAdd(bb, id))
			return false;

		// Render
		const float circleStart = size.x * 0.7f;
		const float circleEnd = size.x;
		const float circleWidth = circleEnd - circleStart;

		window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart, bb.Max.y), bg_col);
		window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart * value, bb.Max.y), fg_col);

		// Render Text on top of bar.
		const char* percent_text = fmt::format("{:d}/100", (int)(value * 100.0f)).c_str();
		const float percent_text_w = ImGui::CalcTextSize(percent_text).x;
		window->DrawList->AddText(ImVec2(bb.Min.x + size_arg.x / 2.0f - percent_text_w - percent_text_w / 2.0f, bb.Min.y), ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), percent_text);

		const float t = g.Time;
		const float r = size.y / 2;
		const float speed = 1.5f;

		const float a = speed * 0;
		const float b = speed * 0.333f;
		const float c = speed * 0.666f;

		const float o1 = (circleWidth + r) * (t + a - speed * (int)((t + a) / speed)) / speed;
		const float o2 = (circleWidth + r) * (t + b - speed * (int)((t + b) / speed)) / speed;
		const float o3 = (circleWidth + r) * (t + c - speed * (int)((t + c) / speed)) / speed;

		window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o1, bb.Min.y + r), r, bg_col);
		window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o2, bb.Min.y + r), r, bg_col);
		window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o3, bb.Min.y + r), r, bg_col);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool Spinner(const char* label, float radius, int thickness, const ImU32& color)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size((radius) * 2, (radius + style.FramePadding.y) * 2);

		const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
		ItemSize(bb, style.FramePadding.y);
		if (!ItemAdd(bb, id))
			return false;

		//- render
		window->DrawList->PathClear();

		int num_segments = 30;
		int start = glm::abs(ImSin(g.Time * 1.8f) * (num_segments - 5));

		const float a_min = IM_PI * 2.0f * ((float)start) / (float)num_segments;
		const float a_max = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;

		const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

		for (int i = 0; i < num_segments; i++) {
			const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
			window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a + g.Time * 8) * radius,
				centre.y + ImSin(a + g.Time * 8) * radius));
		}

		window->DrawList->PathStroke(color, false, thickness);
	}

} //- ImGui