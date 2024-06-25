#include "layer.hpp"
#include <plugin_logging.h>

namespace engine
{
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
			layer.m_shutdown.invoke({});
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void clayers::on_update(float dt)
	{
		for (const auto& layer : m_layers)
		{
			layer.m_update.invoke({}, dt);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void clayers::on_world_render()
	{
		for (const auto& layer : m_layers)
		{
			layer.m_world_render.invoke({});
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void clayers::on_ui_render()
	{
		for (const auto& layer : m_layers)
		{
			layer.m_ui_render.invoke({});
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void clayers::on_post_update(float dt)
	{
		for (const auto& layer : m_layers)
		{
			layer.m_post_update.invoke({}, dt);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void clayers::emplace_new_layer(stringview_t name, rttr::method update, rttr::method world_render, rttr::method ui_render,
		rttr::method post_update, rttr::method on_init, rttr::method on_shutdown)
	{
		m_layers.emplace_back(name, update, world_render, ui_render, post_update, on_init, on_shutdown);
	}

} //- engine