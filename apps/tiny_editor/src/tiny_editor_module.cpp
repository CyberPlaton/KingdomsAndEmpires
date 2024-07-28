#include "tiny_editor_module.hpp"
#include <iostream>
#if MIMALLOC_ENABLE
// #include <mimalloc-new-delete.h>
// #include <mimalloc-override.h>
#endif

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

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	bool ceditor::init()
	{
		auto& docks = cdock_system::instance();

		bool result = true;

		result &= docks.push_back<cmain_menu>();
		result &= docks.push_back<cbottom_panel>();
		result &= docks.push_back<cleft_panel>();
		result &= docks.push_back<cright_panel>();
		result &= docks.push_back<centity_context_panel>();

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ceditor::shutdown()
	{
		auto& docks = cdock_system::instance();

		while (!docks.docks().empty())
		{
			docks.pop_back();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ceditor::on_update(float dt)
	{
		cdock_system::on_update(dt);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ceditor::on_world_render()
	{
		cdock_system::on_world_render();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ceditor::on_ui_render()
	{
		cdock_system::on_ui_render();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ceditor::on_post_update(float dt)
	{
		cdock_system::on_post_update(dt);
	}

} //- editor

RTTR_REGISTRATION
{
	using namespace editor;

	REGISTER_LAYER(ceditor);
}

//------------------------------------------------------------------------------------------------------------------------
int __real_main(int argc, char* argv[])
{
	ZoneScoped;

	AllocConsole();

	logging::clog::instance().init(core::logging_verbosity::logging_verbosity_trace);

	core::set_logger(core_io_error_function);
	sm::set_logger(core_io_error_function);

	engine::cengine::sconfig cfg;

	cfg.m_mode = core::launch_context_editor;
	cfg.m_startup_project = fmt::format("{}/resources/{}", core::cfilesystem::cwd().view(), "default_startup_project.project");

	cfg.m_layers_cfg.emplace_back("cgame");
	cfg.m_layers_cfg.emplace_back("ceditor");

	cfg.m_plugins_cfg.emplace_back("plugin_rttr_example");

	sm::configure(&engine::cengine::instance(),	//- engine class as the application
		(void*)&cfg,							//- engine configuration
		argc,									//- command line args count
		argv);									//- command line args values

	sm::init("tiny editor", 1360, 768, false, false);

// 	core::profile::cpu::get_aggregator()->update();
// 	core::profile::memory::get_aggregator()->update();
// 	sm::profile::gpu::get_aggregator()->update();
// 
// 	auto cpustats = core::profile::cprofiler::cpu_stats();
// 	auto funcstats = core::profile::cprofiler::function_data();
// 	auto memstats = core::profile::cprofiler::memory_stats();
// 	auto gpustats = sm::profile::gpu::get_aggregator()->stats();
// 	auto drawcallstats = sm::profile::gpu::get_aggregator()->drawcall_data();

	sm::run();

	logging::clog::instance().shutdown();

	return 0;
}

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
//------------------------------------------------------------------------------------------------------------------------
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
#if PROFILE_ENABLE
	engine::cprofiler_service::init();
#endif

	//- FIXME: just for decoy, use any type of class so that they wont get optimized out for not being used...
	{
		editor::ceditor editor_decoy;
		cgame game_decoy;
	}

	return __real_main(0, nullptr);
}
#else
//------------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	return __real_main(argc, argv);
}
#endif