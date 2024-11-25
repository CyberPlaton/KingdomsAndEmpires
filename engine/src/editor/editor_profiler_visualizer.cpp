#include "editor_profiler_visualizer.hpp"
#include "elements/editor_element_table.hpp"

namespace editor
{
	namespace
	{
		static constexpr auto C_UPDATE_TIMER	= 5;
		static bool S_OPEN = true;
		constexpr stringview_t C_WINDOW_ID		= "##editor_world_inspector";
		constexpr stringview_t C_CHILD_WINDOW_ID= "##editor_world_inspector";
		constexpr auto C_PROFILER_CHILD_FLAGS	= ImGuiChildFlags_Border;
		constexpr auto C_PROFILER_FLAGS			= ImGuiWindowFlags_MenuBar;
		static vec2_t S_WINDOW_SIZE_MIN = { 250.0f, 100.0f };
		static vec2_t S_WINDOW_SIZE_MAX = { 750.0f, 500.0f };
	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	cprofiler_visualizer::cprofiler_visualizer()
	{
		m_timer.start();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cprofiler_visualizer::on_update(float dt)
	{
		if (m_timer.secs() > C_UPDATE_TIMER)
		{
			update_stats();

			m_timer.start();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cprofiler_visualizer::on_ui_render()
	{
		show_main_window();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cprofiler_visualizer::show_main_window()
	{
		if (const auto window_scope = imgui::cwindow_scope(C_WINDOW_ID.data(), &S_OPEN, C_PROFILER_FLAGS, { 0.0f, 0.0f },
			S_WINDOW_SIZE_MIN, S_WINDOW_SIZE_MAX))
		{
			show_main_menu();

			if (const auto child_scope = imgui::cchild_scope(C_CHILD_WINDOW_ID.data(), { 0.0f, 0.0f }, C_PROFILER_CHILD_FLAGS))
			{
				show_stats();
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cprofiler_visualizer::show_main_menu()
	{
		if (const auto menu_bar_scope = imgui::cmenubar_scope(S_OPEN))
		{
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cprofiler_visualizer::show_stats()
	{
		static constexpr auto C_CPU_STATS_COLUMNS = 6;
		static vector_t<ui::ctable::table_column_data_t> C_CPU_STATS_COLUMN_DATA =
		{
			{ "##Number", ImGuiTableColumnFlags_WidthStretch, 0.1f},
			{ "Function\nName", ImGuiTableColumnFlags_WidthStretch, 0.3f },
			{ "Callcount", ImGuiTableColumnFlags_WidthStretch, 0.1f },
			{ "Time\nAverage", ImGuiTableColumnFlags_WidthStretch, 0.3f },
			{ "Time\nPeak", ImGuiTableColumnFlags_WidthStretch, 0.25f },
			{ "Time\nCumulative", ImGuiTableColumnFlags_WidthStretch, 0.25f },
		};

		const auto size = ImGui::GetContentRegionAvail();

		//- For now, focusing on CPU stats only
		ui::ctable table("##table", &S_OPEN, ui::ctable::options_borders_all);

		table.size({ size.x, size.y });

		if (table.begin(C_CPU_STATS_COLUMNS, C_CPU_STATS_COLUMN_DATA))
		{
			auto i = 0;
			for (const auto& function: m_cpu.m_function_data)
			{
				table.begin_next_row();

				{
					imgui::cgroup_scope group_scope;

					//- Number
					table.begin_column(0);
					ImGui::TextUnformatted(fmt::to_string(++i).data());

					//- Name
					table.begin_column(1);
					ImGui::TextUnformatted(function.m_data.m_name);

					//- Callcount
					table.begin_column(2);
					ImGui::TextUnformatted(fmt::to_string(function.m_callcount).data());

					//- Time Average
					table.begin_column(3);
					ImGui::TextUnformatted(fmt::format("{:.6}ms", function.m_time_average / 1000.0f).data());

					//- Time Peak
					table.begin_column(4);
					ImGui::TextUnformatted(fmt::format("{:.6}ms", function.m_time_peak / 1000.0f).data());

					//- Time Cumulative
					table.begin_column(5);
					ImGui::TextUnformatted(fmt::format("{:.6}ms", function.m_time_cumulative / 1000.0f).data());
				}

				imgui::cui::help_marker_no_question_mark(aggregate_function_stats(function.m_data.m_name));
			}

			table.end();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cprofiler_visualizer::update_stats()
	{
		auto* ps = core::cservice_manager::find<engine::cprofiler_service>();

		m_cpu.m_function_data.clear();

		for (const auto& pair : ps->cpu_function_data())
		{
			m_cpu.m_function_data.emplace_back(pair.second);
		}

		m_cpu.m_cpu_data = ps->cpu_stats();

		algorithm::sort(m_cpu.m_function_data.begin(), m_cpu.m_function_data.end(), [](const auto& a, const auto& b)
			{
				return a.m_time_average > b.m_time_average;
			});
	}

	//------------------------------------------------------------------------------------------------------------------------
	string_t cprofiler_visualizer::aggregate_function_stats(const char* name)
	{
		auto* ps = core::cservice_manager::find<engine::cprofiler_service>();

		const auto& function = ps->cpu_function_data().at(name);

		return fmt::format("Name: '{}'\nCallcount: '{}'\nTime Average: '{}ms'\nTime Peak: '{}ms'\nTime Cumulative: '{}ms'",
			function.m_data.m_name,
			function.m_callcount,
			function.m_time_average / 1000.0f,
			function.m_time_peak / 1000.0f,
			function.m_time_cumulative / 1000.0f);
	}

} //- editor