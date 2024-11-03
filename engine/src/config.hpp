#pragma once
#include <core.h>
#include "version.hpp"

namespace engine
{
	//- Engine configuration. Includes config for spritemancer and core.
	//------------------------------------------------------------------------------------------------------------------------
	struct sconfig final
	{
		static constexpr auto C_DEFAULT_STARTUP_PROJECT = "/default_startup_project.json";

		//- Spritemancer related
		string_t m_title;
		unsigned m_window_width = 0;
		unsigned m_window_height = 0;
		bool m_fullscreen = false;
		bool m_vsync = false;

		//- Engine related
		vector_t<string_t> m_services_cfg;
		vector_t<string_t> m_layers_cfg;
		vector_t<string_t> m_plugins_cfg;
		string_t m_startup_project		= C_DEFAULT_STARTUP_PROJECT;
		launch_context_t m_mode			= core::launch_context_client;

		//- Core related
		core::logging_verbosity m_logging_verbosity			= core::logging_verbosity_trace;
		core::error_report_function_t m_logging_reporter;
		bool m_show_console									= true;

		RTTR_ENABLE();
	};

} //- engine

namespace engine::cfg
{
	//- 
	//------------------------------------------------------------------------------------------------------------------------
	constexpr auto C_MODULE_RESERVE_COUNT = 256;

	//- Maximum count of queries to be processed at a time. When submitting more than that queries are dropped
	//------------------------------------------------------------------------------------------------------------------------
	constexpr auto C_ECS_QUERY_COUNT_MAX = 256;

	//- Whether the ecs world module manager should gather and retain module and system dependency information, such as
	//- module-to-module, system-to-module and system-to-system dependencies etc., all currently loaded systems and modules etc.
	//------------------------------------------------------------------------------------------------------------------------
	constexpr auto C_ECS_MODULES_AND_SYSTEMS_GATHER_INFORMATION = true;

} //- engine::cfg