#include "engine.hpp"
#include <plugin_logging.h>

namespace engine
{
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
	bool cengine::on_init(void* config, argparse::ArgumentParser& args)
	{
		m_result = engine_run_result_ok;
		m_config = std::move(*(sconfig*)config);

		//- do configuration based on provided arguments

		//- register and init services
		logging::log_info("Initialize services...");

		for (const auto& name : m_config.m_services_cfg)
		{
			const auto type = rttr::type::get_by_name(name.c_str());

			if (type.is_valid() && core::cservice_manager::emplace(type))
			{
				logging::log_info(fmt::format("\t'{}' success...", type.get_name().data()));
			}
			else
			{
				logging::log_critical(fmt::format("\t'{}' failed...", name));

				//- fail configuration but let all try to register so we know all those that are bogus
				m_result = engine_run_result_failed_registering_services;
			}
		}

		core::cservice_manager::init();

		//- create and init layers
		logging::log_info("Initializing layers...");

		for (const auto& layer : m_config.m_layers_cfg)
		{
			if (!try_push_layer(layer))
			{
				logging::log_error(fmt::format("\t'{}' failed...", layer));

				//- fail configuration but let all try to register so we know all those that are bogus
				m_result = engine_run_result_failed_pushing_layers;
			}
			else
			{
				logging::log_info(fmt::format("\t'{}' success...", layer));
			}
		}

		m_layers.init();

		return m_result == engine_run_result_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cengine::on_update(float dt)
	{
		CORE_NAMED_ZONE("cengine::on_update");

		core::cservice_manager::on_update(dt);

		ecs::cworld_manager::instance().tick(dt);

		m_layers.on_update(dt);

		m_layers.on_world_render();

		//- TODO: ImGui UI render

		auto v = raylib::GetMousePosition();

		sm::draw_placeholder(0, {v.x, v.y});
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cengine::on_shutdown()
	{
		m_layers.shutdown();

		core::cservice_manager::shutdown();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cengine::try_push_layer(stringview_t name)
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
	}

} //- engine
