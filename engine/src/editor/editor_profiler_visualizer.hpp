#pragma once
#include <core.h>
#include <spritemancer.h>
#include "../services/profiler_service.hpp"

namespace editor
{
	//- Responsible for displaying accumulated data gathered from profiler.
	//------------------------------------------------------------------------------------------------------------------------
	class cprofiler_visualizer final
	{
	public:
		cprofiler_visualizer();
		~cprofiler_visualizer() = default;

		void on_update(float dt);
		void on_ui_render();

	private:
		struct scpu_data final
		{
			engine::cpu_stats_t m_cpu_data;
			vector_t<engine::cpu_function_t> m_function_data;
		};

		scpu_data m_cpu;

		core::ctimer m_timer;

	private:
		void show_main_window();
		void show_main_menu();
		void show_stats();
		void update_stats();
		string_t aggregate_function_stats(const char* name);
	};

} //- editor