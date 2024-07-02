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
		return true;
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
	bool cmodule_service::load_sync(const core::fs::cfileinfo& filepath)
	{
		return load_module(filepath);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<bool> cmodule_service::load_async(const core::fs::cfileinfo& filepath)
	{
		core::cfuture_type<bool> result = core::casync::launch_async([&]() -> bool
			{

				return load_module(filepath);

			}).share();

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmodule_service::load_module(const core::fs::cfileinfo& filepath)
	{
		auto& vfs = core::cservice_manager::get<core::fs::cvirtual_filesystem>();

		//- Load module definition file
		if (auto file = vfs.open(filepath, core::file_mode_read); file)
		{
			//- Load data and parse from json to object
			const auto size = file->size();
			auto data = core::cmemory::make_ref(size);

			if (file->read(data->data(), size) == size)
			{
				auto module = core::io::from_json_blob<cmodule>(data->data(), size);

				//- After module is loaded, we post loading requests for assets and resources

				//- Lastly, we recursively load dependent modules
				for (const auto& def : module.definitions())
				{
					for (const auto& dep : def.m_dependencies)
					{
						if (!load_sync(stringview_t(dep)))
						{
							logging::log_error(fmt::format("Failed loading dependent module '{}' while loading '{}'", dep, module.name()));

							return false;
						}
					}
				}
				return true;
			}
		}
		return false;
	}

} //- io