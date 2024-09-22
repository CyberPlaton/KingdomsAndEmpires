#include "plugin_service.hpp"

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	cplugin_service::~cplugin_service()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cplugin_service::on_start()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cplugin_service::on_shutdown()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cplugin_service::on_update(float dt)
	{
		core::cscope_mutex m(m_mutex);

		m_timer.start();

		while (m_timer.millisecs() < dt)
		{
			//-- Load plugins first
			if (!m_loading_queue.empty())
			{
				const auto& plugin = m_loading_queue.top(); m_loading_queue.pop();

				load_plugin(plugin);
			}

			//-- Unload plugins next
			if (!m_unloading_queue.empty())
			{
				const auto& plugin = m_unloading_queue.top(); m_unloading_queue.pop();

				unload_plugin(plugin);
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cplugin_service::load_async(stringview_t filename)
	{
		core::cscope_mutex m(m_mutex);

		m_loading_queue.push(filename.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cplugin_service::unload_async(stringview_t filename)
	{
		core::cscope_mutex m(m_mutex);

		m_unloading_queue.push(filename.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cplugin_service::load(stringview_t filename)
	{
		load_plugin(filename);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cplugin_service::unload(stringview_t filename)
	{
		unload_plugin(filename);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cplugin_service::load_plugin(stringview_t filename)
	{
		if (m_plugins.find(filename.data()) != m_plugins.end())
		{
			return;
		}

		//-- Load plugin information so we can load dependencies if required
		if (const auto json = core::fs::load_text_from_file(fmt::format("{}/resources/{}.plugin", core::cfilesystem::cwd().view(), filename.data())); json)
		{
			auto info = core::io::from_json_blob(rttr::type::get<cplugin::sconfig>(), json->data(), json->size()).get_value<cplugin::sconfig>();

			auto plugin = std::make_shared<cplugin>(std::move(info));

			//- Check for dependencies
			if (!plugin->config().m_dependencies.empty())
			{
				for (const auto& dep : plugin->config().m_dependencies)
				{
					load_plugin(dep);
				}
			}

			if (plugin->load())
			{
				m_plugins[filename.data()] = std::move(plugin);

				log_info(fmt::format("Successfully loaded plugin '{}'", filename.data()));
				return;
			}
			else
			{
				log_error(fmt::format("Failed to load plugin '{}' with message '{}'", filename.data(), plugin->error()));
			}
		}

		log_error(fmt::format("Failed to load plugin '{}', as the plugin definition file was not found!", filename.data()));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cplugin_service::unload_plugin(stringview_t filename)
	{
		auto it = m_plugins.find(filename.data());

		if (it != m_plugins.end())
		{
			m_plugins.erase(it);
		}
	}

} //- engine

RTTR_REGISTRATION
{
	using namespace engine;

	rttr::cregistrator<cplugin_service, rttr::detail::as_raw_pointer>("cplugin_service")
		;
}