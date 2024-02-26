#include "kingdoms_editor_module.hpp"
#include <iostream>

void core_io_error_function(uint8_t level, const std::string& message)
{
	switch (level)
	{
	case SPDLOG_LEVEL_TRACE:
	{
		logging::log_trace(message);
		break;
	}
	case SPDLOG_LEVEL_DEBUG:
	{
		logging::log_debug(message);
		break;
	}
	case SPDLOG_LEVEL_INFO:
	{
		logging::log_info(message);
		break;
	}
	case SPDLOG_LEVEL_WARN:
	{
		logging::log_warn(message);
		break;
	}
	case SPDLOG_LEVEL_ERROR:
	{
		logging::log_error(message);
		break;
	}
	case SPDLOG_LEVEL_CRITICAL:
	{
		logging::log_critical(message);
		break;
	}
	}
}

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	AllocConsole();

	logging::init();

	engine::cengine::sconfig cfg;
	cfg.m_service_cfg.m_services.emplace_back("ccamera_manager");

	cfg.m_layer_cfg.emplace_back("cgame");
	cfg.m_layer_cfg.emplace_back("ceditor");

	cfg.m_window_cfg.m_title = "Kingdoms Editor";
	cfg.m_window_cfg.m_width = 1280;
	cfg.m_window_cfg.m_height = 1024;
	cfg.m_window_cfg.m_target_fps = 0;
	cfg.m_window_cfg.m_flags = sm::window_flag_vsync | sm::window_flag_show;

	if (engine::cengine::instance().configure(cfg/*"config.json"*/) == engine::engine_run_result_ok)
	{
		engine::cengine::instance().run();
	}
	return 0;
}
#else
int main(int argc, char* argv[])
{
	logging::init();

	engine::cengine::sconfig cfg;
	cfg.m_service_cfg.m_services.emplace_back("ccamera_manager");
	cfg.m_layer_cfg.emplace_back("ceditor");
	cfg.m_window_cfg.m_title = "Kingdoms Editor";
	cfg.m_window_cfg.m_width = 1280;
	cfg.m_window_cfg.m_height = 1024;
	cfg.m_window_cfg.m_target_fps = 0;
	cfg.m_window_cfg.m_flags = sm::window_flag_vsync | sm::window_flag_show;

	if (engine::cengine::instance().configure(cfg/*"config.json"*/) == engine::engine_run_result_ok)
	{
		engine::cengine::instance().run();
	}
	return 0;
}
#endif

void ceditor::on_update(float dt)
{

}

void ceditor::on_world_render()
{

}

void ceditor::on_ui_render()
{

}

void ceditor::on_post_update(float dt)
{

}
