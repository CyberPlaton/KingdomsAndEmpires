#include <engine.h>
#include "src/tiny_editor_module.hpp"
#include <iostream>
#if MIMALLOC_ENABLE
#include <mimalloc-override.h>
#endif

void core_io_error_function(uint8_t level, const std::string& message);


//- Configure possible arguments and their default values etc.
//------------------------------------------------------------------------------------------------------------------------
void configure_args(const args_ref_t& args)
{

}

//------------------------------------------------------------------------------------------------------------------------
engine::sconfig tiny_editor_main(const args_ref_t& args)
{
	//- Dummy. Otherwise unknown editor layer will be optimized away
	{
		editor::ceditor e;
	}

	engine::sconfig cfg;

	cfg.m_mode = core::launch_context_editor;
	cfg.m_startup_project = "/default_startup_project.json";
	cfg.m_layers_cfg.emplace_back("ceditor");
	cfg.m_plugins_cfg.emplace_back("plugin_rttr_example");

	cfg.m_title = "tiny editor";
	cfg.m_window_width = 1360;
	cfg.m_window_height = 768;
	cfg.m_fullscreen = false;
	cfg.m_vsync = false;

	cfg.m_logging_reporter = core_io_error_function;
	cfg.m_logging_verbosity = core::logging_verbosity_trace;
	cfg.m_show_console = true;

	return cfg;
}

ENGINE_MAIN(tiny_editor_main, configure_args);

//------------------------------------------------------------------------------------------------------------------------
void core_io_error_function(uint8_t level, const std::string& message)
{
	switch (level)
	{
	case SPDLOG_LEVEL_TRACE:
	{
		log_trace(message);
		break;
	}
	case SPDLOG_LEVEL_DEBUG:
	{
		log_debug(message);
		break;
	}
	case SPDLOG_LEVEL_INFO:
	{
		log_info(message);
		break;
	}
	case SPDLOG_LEVEL_WARN:
	{
		log_warn(message);
		break;
	}
	case SPDLOG_LEVEL_ERROR:
	{
		log_error(message);
		break;
	}
	case SPDLOG_LEVEL_CRITICAL:
	{
		log_critical(message);
		break;
	}
	}
}