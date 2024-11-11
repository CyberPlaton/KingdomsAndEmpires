#include <engine.h>
#include <plugin_camera_system.h>
#include <kingdoms_and_empires.h>
#include <lua.h>
#include <luabridge.h>
#include <iostream>

//------------------------------------------------------------------------------------------------------------------------
void core_io_error_function(uint8_t level, const std::string& message);

//- Configure possible arguments and their default values etc.
//------------------------------------------------------------------------------------------------------------------------
void configure_args(const args_ref_t& args)
{

}

//------------------------------------------------------------------------------------------------------------------------
engine::sconfig kingdoms_and_empires_main(const args_ref_t& args)
{
	engine::sconfig cfg;

	cfg.m_mode = core::launch_context_client;
	cfg.m_title = "kingdoms & empires";
	cfg.m_fullscreen = true;
	cfg.m_vsync = true;

	cfg.m_logging_reporter = core_io_error_function;
	cfg.m_logging_verbosity = core::logging_verbosity_warn;
	cfg.m_show_console = false;

	return cfg;
}

ENGINE_MAIN(kingdoms_and_empires_main, configure_args);

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