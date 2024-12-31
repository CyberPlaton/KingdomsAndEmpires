#include "sm_testbed_module.hpp"
#include <iostream>

void core_io_error_function(uint8_t level, const std::string& message);
void configure_args(const args_ref_t& args);

//------------------------------------------------------------------------------------------------------------------------
bool cspritemancer::init()
{
	return true;
}

//------------------------------------------------------------------------------------------------------------------------
void cspritemancer::shutdown()
{

}

//------------------------------------------------------------------------------------------------------------------------
void cspritemancer::on_update(float dt)
{
}

//------------------------------------------------------------------------------------------------------------------------
void cspritemancer::on_world_render()
{

}

//------------------------------------------------------------------------------------------------------------------------
void cspritemancer::on_ui_render()
{

}

//------------------------------------------------------------------------------------------------------------------------
void cspritemancer::on_post_update(float dt)
{

}

//------------------------------------------------------------------------------------------------------------------------
engine::sconfig spritemancer_main(const args_ref_t& args)
{
	engine::sconfig cfg;

	cfg.m_startup_project = "/spritemancer.project";
	cfg.m_layers_cfg.emplace_back("cspritemancer");
	cfg.m_mode = core::launch_context_client;
	cfg.m_title = "spritemancer";
	cfg.m_fullscreen = false;
	cfg.m_vsync = false;
	cfg.m_window_width = 1680;
	cfg.m_window_height = 920;

#if CORE_DEBUG
	cfg.m_logging_verbosity = core::logging_verbosity_debug;
	cfg.m_show_console = true;
#else
	cfg.m_logging_verbosity = core::logging_verbosity_error;
	cfg.m_show_console = false;
#endif
	cfg.m_logging_reporter = core_io_error_function;

	return cfg;
}

ENGINE_MAIN(spritemancer_main, configure_args);

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

//------------------------------------------------------------------------------------------------------------------------
void configure_args(const args_ref_t& args)
{

}

RTTR_REGISTRATION
{
	REGISTER_LAYER(cspritemancer);
}