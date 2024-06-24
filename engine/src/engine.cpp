#include "engine.hpp"
#include <plugin_logging.h>

namespace engine
{
	namespace
	{
	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	clayers::slayer_data::slayer_data(stringview_t name, rttr::method update, rttr::method world_render, rttr::method ui_render,
		rttr::method post_update, rttr::method on_init, rttr::method on_shutdown) :
		m_name(name.data()), m_update(update), m_world_render(world_render), m_ui_render(ui_render), m_post_update(post_update),
		m_init(on_init), m_shutdown(on_shutdown)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void clayers::init()
	{
		vector_t<unsigned> failed_layers; failed_layers.reserve(m_layers.size());

		for (auto i = 0u; i < m_layers.size(); ++i)
		{
			const auto& meth = m_layers[i].m_init;

			CORE_ASSERT(meth.is_valid(), "Invalid operation. Layer init function must be present!");

			logging::log_info(fmt::format("\t'{}'", m_layers[i].m_name));

			if (const auto var = meth.invoke({}); !var.to_bool())
			{
				logging::log_error("\t\t...fail. This layer will be removed!");

				failed_layers.push_back(i);
			}
			else
			{
				logging::log_info("\t\t...ok");
			}
		}

		//- Remove layers that have failed and should not be ran
		for (auto& idx : failed_layers)
		{
			algorithm::erase_at_index(m_layers, idx);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void clayers::shutdown()
	{
		for (const auto& layer : m_layers)
		{
			execute_method(layer.m_shutdown);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void clayers::on_update(float dt)
	{
		for (const auto& layer : m_layers)
		{
			execute_method(layer.m_update);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void clayers::on_world_render()
	{
		for (const auto& layer : m_layers)
		{
			execute_method(layer.m_world_render);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void clayers::on_ui_render()
	{
		for (const auto& layer : m_layers)
		{
			execute_method(layer.m_ui_render);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void clayers::on_post_update(float dt)
	{
		for (const auto& layer : m_layers)
		{
			execute_method(layer.m_post_update);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void clayers::emplace_new_layer(stringview_t name, rttr::method update, rttr::method world_render, rttr::method ui_render,
		rttr::method post_update, rttr::method on_init, rttr::method on_shutdown)
	{
		m_layers.emplace_back(name, update, world_render, ui_render, post_update, on_init, on_shutdown);
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
			logging::log_info(fmt::format("\t'{}'", name));

			const auto type = rttr::type::get_by_name(name.c_str());

			if (type.is_valid() && core::cservice_manager::emplace(type))
			{
				logging::log_info("\t\t...ok");
			}
			else
			{
				logging::log_critical("\t\t...fail");

				//- fail configuration but let all try to register so we know all those that are bogus
				m_result = engine_run_result_failed_registering_services;
			}
		}

		core::cservice_manager::init();

		//- create and init layers
		logging::log_info("Pushing layers...");

		for (const auto& layer : m_config.m_layers_cfg)
		{
			logging::log_info(fmt::format("\t'{}'", layer));

			if (!try_push_layer(layer))
			{
				logging::log_error("\t\t...fail");

				//- fail configuration but let all try to register so we know all those that are bogus
				m_result = engine_run_result_failed_pushing_layers;
			}
			else
			{
				logging::log_info("\t\t...ok");
			}
		}

		logging::log_info("Initializing layers...");

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

		//- Example on how rendering is possible from update function and from different threads
		auto v = raylib::GetMousePosition();

		sm::draw_placeholder(0, {v.x, v.y});
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cengine::on_imgui()
	{
		m_layers.on_ui_render();

		ImGui::ShowDemoWindow();
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
		auto result = false;

		if (auto type = rttr::type::get_by_name(name.data()); type.is_valid())
		{
			//- check that at least one function is present
			auto update_method			= type.get_method(slayer::C_LAYER_UPDATE_FUNC_NAME.data());
			auto world_render_method	= type.get_method(slayer::C_LAYER_WORLD_RENDER_FUNC_NAME.data());
			auto ui_render_method		= type.get_method(slayer::C_LAYER_UI_RENDER_FUNC_NAME.data());
			auto post_update_method		= type.get_method(slayer::C_LAYER_POST_UPDATE_FUNC_NAME.data());
			auto init_method			= type.get_method(slayer::C_LAYER_INIT_FUNC_NAME.data());
			auto shutdown_method		= type.get_method(slayer::C_LAYER_SHUTDOWN_FUNC_NAME.data());

			m_layers.emplace_new_layer(name, update_method, world_render_method, ui_render_method, post_update_method, init_method, shutdown_method);

			//- Check that all methods are valid and issue a warning if not, as this can be detremental to performance
			if (!(update_method.is_valid() && world_render_method.is_valid() && ui_render_method.is_valid() &&
				post_update_method.is_valid() && init_method.is_valid() && shutdown_method.is_valid()))
			{
				logging::log_error("\t\tA function not defined, this can be detremental to performance!");

				//- Check which functions are defined and which not
				for (const auto& func_name : slayer::C_LAYER_FUNC_NAMES)
				{
					if (const auto& meth = type.get_method(func_name.data()); meth.is_valid())
					{
						logging::log_info(fmt::format("\t\t\t'{}'... OK", meth.get_signature().data()));
					}
					else
					{
						logging::log_warn(fmt::format("\t\t\t'{}'... UNDEFINED", func_name.data()));
					}
				}
			}
			//- Check that all methods are declared as static, if not undefined behavior will occurr, so this is an error actually.
			else if (!(update_method.is_static() && world_render_method.is_static() && ui_render_method.is_static() &&
				post_update_method.is_static() && init_method.is_static() && shutdown_method.is_static()))
			{
				logging::log_error("\t\tA function not decalred 'static', this will result in undefined behavior!");

				//- Check which functions are static and which not
				for (const auto& func_name : slayer::C_LAYER_FUNC_NAMES)
				{
					if (const auto& meth = type.get_method(func_name.data()); meth.is_valid())
					{
						logging::log_info(fmt::format("\t\t\t'{}'... OK", meth.get_signature().data()));
					}
					else
					{
						logging::log_warn(fmt::format("\t\t\t'{}'... NOT 'static'", func_name.data()));
					}
				}
			}
			else
			{
				result = true;
			}
		}
		return result;
	}

} //- engine
