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
	void cengine::clayers::init()
	{
		for (const auto& m : m_layer_init)
		{
			m.invoke({});
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cengine::clayers::shutdown()
	{
		for (const auto& m : m_layer_shutdown)
		{
			m.invoke({});
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cengine::clayers::on_update(float dt)
	{
		for (const auto& m : m_layer_update)
		{
			m.invoke({}, dt);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cengine::clayers::on_world_render()
	{
		for (const auto& m : m_layer_world_render)
		{
			m.invoke({});
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cengine::clayers::on_ui_render()
	{
		for (const auto& m : m_layer_ui_render)
		{
			m.invoke({});
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cengine::clayers::on_post_update(float dt)
	{
		for (const auto& m : m_layer_post_update)
		{
			m.invoke({}, dt);
		}
	}

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
			core::cservice_manager::init();

			if (!sm::init(m_config.m_window_cfg))
			{
				m_result = engine_run_result_failed_starting_spritemancer;
			}

			//- initialize layers
			if (!init_layers(m_config.m_layer_cfg))
			{
				m_result = engine_run_result_failed_pushing_layers;
			}

			m_layers.init();
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

			if (!json.empty())
			{
				config = core::io::from_json_string<sconfig>(json);

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
	engine_run_result cengine::run()
	{
		if (m_result != engine_run_result_ok)
		{
			return m_result;
		}

		//- enter engine main loop
		while (!raylib::WindowShouldClose())
		{
			ZoneScopedN("Engine Frame");

			//- NOTE: reconsider how to handle events from hardware and game/engine/editor internal
			if (raylib::IsWindowResized())
			{
				sm::ctx().on_resize(raylib::GetScreenWidth(), raylib::GetScreenHeight());
			}

			//- TODO: implement ctimestep class for variable or fixed delta time
			{
				//- general update and deferred rendering calls
				core::cservice_manager::on_update(0.016f);

				ecs::cworld_manager::instance().tick(0.016f);

				m_layers.on_update(0.016f);
			}

			{
				//- raylib frame processing deferred rendering class
				sm::begin_drawing();

				m_layers.on_world_render();

				sm::end_frame();
			}

			{
				//- ImGui ui frame
				sm::ui_frame();

				m_layers.on_ui_render();

				ImGui::ShowDemoWindow();

				sm::end_ui_frame();

				sm::end_drawing();
			}
		}

		m_layers.shutdown();

		sm::shutdown();

		core::cservice_manager::shutdown();

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
		logging::log_info("Initialize services...");

		for (const auto& name : m_config.m_service_cfg.m_services)
		{
			const auto type = rttr::type::get_by_name(name.c_str());

			if (type.is_valid() && core::cservice_manager::emplace(type))
			{
				logging::log_info(fmt::format("\t'{}' success...", type.get_name().data()));
			}
			else
			{
				logging::log_critical(fmt::format("\t'{}' failed...", type.get_name().data()));

				//- fail configuration but let all try to register so we know all that are bugous
				m_result = engine_run_result_failed_registering_services;
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cengine::init_layers(const clayers::sconfig& cfg)
	{
		logging::log_info("Initializing layers...");

		bool result = true;

		const auto try_push_layer = [this](std::string_view name) -> bool
			{
				if (auto type = rttr::type::get_by_name(name.data()); type.is_valid())
				{
					//- check that at least one function is present
					auto update_method = type.get_method(slayer::C_LAYER_UPDATE_FUNC_NAME.data());
					auto world_render_method = type.get_method(slayer::C_LAYER_WORLD_RENDER_FUNC_NAME.data());
					auto ui_render_method = type.get_method(slayer::C_LAYER_UI_RENDER_FUNC_NAME.data());
					auto post_update_method = type.get_method(slayer::C_LAYER_POST_UPDATE_FUNC_NAME.data());
					auto init_method = type.get_method(slayer::C_LAYER_INIT_FUNC_NAME.data());
					auto shutdown_method = type.get_method(slayer::C_LAYER_SHUTDOWN_FUNC_NAME.data());

					if (update_method.is_valid())
					{
						m_layers.m_layer_update.emplace_back(update_method);
					}
					if (world_render_method.is_valid())
					{
						m_layers.m_layer_world_render.emplace_back(world_render_method);
					}
					if (ui_render_method.is_valid())
					{
						m_layers.m_layer_ui_render.emplace_back(ui_render_method);
					}
					if (post_update_method.is_valid())
					{
						m_layers.m_layer_post_update.emplace_back(post_update_method);
					}
					if (init_method.is_valid())
					{
						m_layers.m_layer_init.emplace_back(init_method);
					}
					if (shutdown_method.is_valid())
					{
						m_layers.m_layer_shutdown.emplace_back(shutdown_method);
					}

					return update_method.is_valid() || world_render_method.is_valid() ||
						ui_render_method.is_valid() || post_update_method.is_valid() ||
						init_method.is_valid() || shutdown_method.is_valid();
				}
				return false;
			};

		for (const auto& layer : cfg.m_layers)
		{
			if (!try_push_layer(layer))
			{
				logging::log_error(fmt::format("\t'{}' failed...", layer));

				result = false;
			}
			else
			{
				logging::log_info(fmt::format("\t'{}' success...", layer));
			}
		}
		return result;
	}

} //- engine