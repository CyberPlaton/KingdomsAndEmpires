#include "main.hpp"
#include <engine.h>

namespace engine
{
	namespace detail
	{
		//------------------------------------------------------------------------------------------------------------------------
		int engine_main_entry(engine_main_func_t function, engine_args_config_func_t args_config, int argc, char* argv[])
		{
			//- Enable profiler, either tracy or engine one
#if PROFILE_ENABLE
	#if CORE_PLATFORM_WINDOWS && TRACY_ENABLE
	#else
			engine::cprofiler_service::init();
	#endif
			CORE_NAMED_ZONE(engine_main_entry);
#endif
			core::sentry::init();

			//- Parse arguments and create a configuration
			args_ref_t args = core::cargs::make_ref(engine::sinfo::C_NAME, engine::sinfo::C_VERSION);

			//- Let everyone configure arguments and then parse them
			cengine::configure_args(args);
			sm::configure_args(args);
			args_config(args);

			if (!args->parse(argc, argv))
			{
				return -1;
			}

			//- Execute client entry and let it configure engine, spritemancer, core and application etc.
			auto cfg = function(args);

			if (cfg.m_show_console)
			{
#if CORE_PLATFORM_WINDOWS
				AllocConsole();		
#endif
			}

			//- Initialize logging as fast as possible and set reporters
			logging::clog::instance().init(cfg.m_logging_verbosity);
			core::serror_reporter::set_logger(cfg.m_logging_reporter);
			sm::set_logger(cfg.m_logging_reporter);

			//- Prepare engine and spritemancer and run application
			cengine::prepare(cfg);
			sm::prepare(&engine::cengine::instance(), (void*)&cfg);
			const auto result = static_cast<int>(sm::run(cfg.m_title.data(), cfg.m_window_width, cfg.m_window_height, cfg.m_fullscreen, cfg.m_vsync));

			//- Shut everything down in reverse order of initialization
			logging::clog::instance().shutdown();
			core::sentry::shutdown();

			return result;
		}

	} //- detail

} //- engine
