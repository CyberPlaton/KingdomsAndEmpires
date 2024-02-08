#include "engine.hpp"
#include <plugin_logging.h>

namespace engine
{
	namespace
	{
		void configure_engine_arguments(argparse::ArgumentParser& args)
		{
			//- common arguments available everywhere
			args.add_argument("--world")
				.default_value("");

			//- configuration specific arguments
#if DEBUG
			args.add_argument("--console")
				.default_value(true);
#else
#endif
		}

	} //- unnamed


	//------------------------------------------------------------------------------------------------------------------------
	cengine::~cengine()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	engine_run_result cengine::configure(sconfig cfg, int argc /*= 0*/, char* argv[] /*= {}*/)
	{
		m_result = engine_run_result_ok;

		//- parse arguments and configure self
		argparse::ArgumentParser args;

		configure_engine_arguments(args);

		handle_arguments(args, argc, argv);

		//- initialize systems and services and prepare for running
		if (m_result == engine_run_result_ok)
		{
			register_services(args);

			if (!sm::init(cfg.m_window_cfg))
			{
				m_result = engine_run_result_failed_starting_spritemancer;
			}
		}

		return m_result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	engine_run_result cengine::run() const
	{
		if (m_result != engine_run_result_ok)
		{
			return m_result;
		}

		//- enter engine main loop
		while (!raylib::WindowShouldClose())
		{
			//- NOTE: reconsider how to handle events from hardware and game/engine/editor internal
			if (raylib::IsWindowResized())
			{
				sm::ctx().on_resize(raylib::GetScreenWidth(), raylib::GetScreenHeight());
			}

			sm::begin_drawing(camera);

			sm::end_frame();

			sm::ui_frame();

			sm::end_ui_frame();

			sm::end_drawing();
		}


		return m_result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cengine::handle_arguments(argparse::ArgumentParser& args, int argc, char* argv[])
	{
		//- argparse throws on errorss
		try
		{
			args.parse_args(argc, argv);
		}
		catch (const std::runtime_error& err)
		{
			logging::log_critical(fmt::format("Failed parsing arguments: '{}'", err.what()));

			m_result = engine_run_result_failed_parsing_arguments;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cengine::register_services(argparse::ArgumentParser& args)
	{
		if (m_result != engine_run_result_ok)
		{
			return;
		}
	}

} //- engine