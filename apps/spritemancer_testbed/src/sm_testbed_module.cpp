#include "sm_testbed_module.hpp"
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
	cfg.m_service_cfg.m_services.emplace_back("cthread_service");

	cfg.m_layer_cfg.m_layers.emplace_back("cgame");

	cfg.m_window_cfg.m_title = "Spritemancer Testbed";
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
	cfg.m_service_cfg.m_services.emplace_back("cthread_service");

	cfg.m_layer_cfg.m_layers.emplace_back("cgame");
	cfg.m_layer_cfg.m_layers.emplace_back("ceditor");

	cfg.m_window_cfg.m_title = "Tiny Editor";
	cfg.m_window_cfg.m_width = 1280;
	cfg.m_window_cfg.m_height = 1024;
	cfg.m_window_cfg.m_target_fps = 0;
	cfg.m_window_cfg.m_flags = sm::window_flag_vsync | sm::window_flag_show;

	if (engine::cengine::instance().configure(cfg/*"config.json"*/) == engine::engine_run_result_ok)
	{
		ecs::cworld_manager::instance().create("World");

		auto& w = ecs::cworld_manager::instance().active();

		//- can´t use more than what was configured as foreground threads
		w.use_threads(engine::cthread_service::hardware_threads() / 2);

		//- import module with required components
		w.mm().import_module<module_example::cmy_second_module>();

		//- create several entities for testing
		auto walther = w.em().create_entity();
		auto adolf = w.em().create_entity();
		auto manfred = w.em().create_entity();
		auto hans = w.em().create_entity();
		auto peter = w.em().create_entity();

		walther.add<module_example::stargeting_component>();
		adolf.add<module_example::stargeting_component>();
		manfred.add<module_example::stargeting_component>();
		hans.add<module_example::stargeting_component>();
		peter.add<module_example::stargeting_component>();

		engine::cengine::instance().run();
	}
	return 0;
}
#endif