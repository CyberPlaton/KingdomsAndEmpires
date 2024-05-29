#include "tiny_editor_module.hpp"
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
int __real_main(int argc, char* argv[])
{
	AllocConsole();

	logging::init();

	engine::cengine::sconfig cfg;
	cfg.m_services_cfg.emplace_back("cthread_service");
	cfg.m_services_cfg.emplace_back("cevent_service");

	cfg.m_layers_cfg.emplace_back("cgame");
	cfg.m_layers_cfg.emplace_back("ceditor");

	sm::set_logger(core_io_error_function);

	sm::configure(&engine::cengine::instance(),	//- engine class as the application
		(void*)&cfg,							//- engine configuration
		argc,									//- command line args count
		argv);									//- command line args values

	sm::init("tiny editor", 1360, 768, false, false);

	sm::run();

	return 0;
}

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	return __real_main(0, nullptr);
}
#else
int main(int argc, char* argv[])
{
	return __real_main(argc, argv);
}
#endif