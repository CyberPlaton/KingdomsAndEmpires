#include "kingdoms_and_empires_module.hpp"
#include <iostream>

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
int __real_main(int argc, char* argv[])
{
	AllocConsole();

	logging::clog::instance().init(core::logging_verbosity::logging_verbosity_debug);

	core::set_logger(core_io_error_function);
	sm::set_logger(core_io_error_function);

	engine::cengine::sconfig cfg;

	cfg.m_layers_cfg.emplace_back("cgame");

	sm::configure(&engine::cengine::instance(),	//- engine class as the application
		(void*)&cfg,							//- engine configuration
		argc,									//- command line args count
		argv);									//- command line args values

	sm::run("Kingdoms & Empires", 1280, 1024, true, true);

	logging::clog::instance().shutdown();

	return 0;
}

//------------------------------------------------------------------------------------------------------------------------
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