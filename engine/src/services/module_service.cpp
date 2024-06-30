#include "module_service.hpp"
#include <plugin_logging.h>

namespace io
{
	namespace
	{
		constexpr stringview_t C_MODULE_ON_LOAD_FUNC	= "on_load";
		constexpr stringview_t C_MODULE_ON_UNLOAD_FUNC	= "on_unload";

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	cmodule_service::~cmodule_service()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmodule_service::on_start()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmodule_service::on_shutdown()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmodule_service::on_update(float dt)
	{
		m_timer.start();

		while (m_timer.millisecs() < dt)
		{
			//- serve loading requests
			if (!m_to_load.empty())
			{
				core::cscope_mutex m(m_mutex);

				const auto module = m_to_load.top(); m_to_load.pop();
			}

			//- server unloading requests
			if (!m_to_unload.empty())
			{
				core::cscope_mutex m(m_mutex);

				const auto module = m_to_unload.top(); m_to_unload.pop();
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmodule_service::load_module(stringview_t type_name, const core::fs::cfileinfo& filepath)
	{
		if (const auto type = rttr::type::get_by_name(type_name.data()); type.is_valid())
		{
			return load_module(type, filepath);

			auto m = type.create({});

			auto on_load = rttr::detail::get_method<core::cmodule>(C_MODULE_ON_LOAD_FUNC);

			if (const auto result = on_load.invoke(m).convert<bool>(); result)
			{
				m_modules[name.data()] = std::move(m);

				logging::log_info(fmt::format("Successfully loaded module '{}'", name.data()));

				return true;
			}

			logging::log_warn(fmt::format("Failed to initialize module '{}'", name.data()));
		}

		logging::log_error(fmt::format("Failed to load module '{}'", name.data()));

		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmodule_service::load_module(rttr::type type, const core::fs::cfileinfo& filepath)
	{

	}

} //- io