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

		m_config = std::move(cfg);

		//- parse arguments and configure self
		argparse::ArgumentParser args;

		configure_engine_arguments(args);

		handle_arguments(args, argc, argv);

		//- initialize systems and services and prepare for running
		if (m_result == engine_run_result_ok)
		{
			register_services(args);

			//- default service init
			cservice_manager::init();

			if (!sm::init(m_config.m_window_cfg))
			{
				m_result = engine_run_result_failed_starting_spritemancer;
			}
		}

		return m_result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	engine_run_result cengine::configure(const core::cpath& cfg, int argc /*= 0*/, char* argv[] /*= {}*/)
	{
		sconfig config;

		if (cfg.exists() && cfg.is_file())
		{
			auto config_text = core::cfile::load_text(cfg.view());

			std::string json(config_text);

			if (io::from_json(json, config))
			{
				return configure(config, argc, argv);
			}
			else
			{
				m_result = engine_run_result_failed_parsing_invalid_config;
			}
		}
		else
		{
			m_result = engine_run_result_failed_loading_config;
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

			cservice_manager::on_update(0.016f);

			const auto* camera_manager = cservice_manager::find<sm::icamera_manager>("ccamera_manager");
			auto* camera = camera_manager->has_active_camera() ? camera_manager->active_camera() : camera_manager->default_camera();

			sm::begin_drawing(camera);

			sm::end_frame();

			sm::ui_frame();

			ImGui::ShowDemoWindow();

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
			if (argc > 0 && argv)
			{
				args.parse_args(argc, argv);
			}
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

		//- register common engine services
		for (const auto& name : m_config.m_service_cfg.m_services)
		{
			auto type = rttr::type::get_by_name(name.c_str());

			if (type.is_valid() && cservice_manager::emplace(type))
			{
				logging::log_info(fmt::format("Registered service: '{}'", type.get_name().data()));
			}
			else
			{
				logging::log_critical(fmt::format("Failed registering service: '{}'", type.get_name().data()));

				//- fail configuration but let all try to register so we know all that are bugous
				m_result = engine_run_result_failed_registering_services;
			}
		}
	}

} //- engine