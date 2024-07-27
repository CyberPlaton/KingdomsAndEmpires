#include "module_service.hpp"

namespace io
{
	//------------------------------------------------------------------------------------------------------------------------
	cmodule_service::~cmodule_service()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmodule_service::on_start()
	{
		m_modules.reserve(engine::cfg::C_MODULE_RESERVE_COUNT);

		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmodule_service::on_shutdown()
	{
		//- TODO: forcefully unload all modules
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmodule_service::on_update(float dt)
	{
		core::cscope_mutex m(m_mutex);

		m_timer.start();

		while (m_timer.millisecs() < dt)
		{
			//- serve loading requests
			if (!m_to_load.empty())
			{
				const auto& module = m_to_load.top(); m_to_load.pop();

				load_module(module);
			}

			//- serve unloading requests
			if (!m_to_unload.empty())
			{
				const auto& module = m_to_unload.top(); m_to_unload.pop();

				unload_module(module);
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmodule_service::load(const core::fs::cfileinfo& filepath)
	{
		core::cscope_mutex m(m_mutex);

		m_to_load.push(filepath);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmodule_service::unload(const core::fs::cfileinfo& filepath)
	{
		core::cscope_mutex m(m_mutex);

		m_to_unload.push(filepath);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmodule_service::load_module(const core::fs::cfileinfo& filepath)
	{
		auto& vfs = core::cservice_manager::get<core::fs::cvirtual_filesystem>();
		auto has_deps = false;

		//- Load module definition file
		if (auto file = vfs.open(filepath, core::file_mode_read); file)
		{
			//- Load data and parse from json to object
			if (const auto& memory = file->read_async().wait(); memory)
			{
				auto module = core::io::from_json_blob<cmodule>(memory->data(), memory->size());

				//- If this module has dependencies, we have to load them first and afterwards the module itself
				for (const auto& def : module.definitions())
				{
					if (!def.m_dependencies.empty())
					{
						has_deps = true;

						for (const auto& dep : def.m_dependencies)
						{
							m_to_load.push({ dep.data() });
						}
					}
				}

				if (has_deps)
				{
					m_to_load.push(filepath);

					return;
				}

				//- Post loading requests for assets and resources
				for (const auto& def : module.definitions())
				{

				}


				//- TODO: After module is loaded, we post loading requests for assets and resources

				for (const auto& def : module.definitions())
				{
					for (const auto& dep : def.m_dependencies)
					{
						//- Push dependency modules to be loaded accross next frames
						m_to_load.push({ dep.data() });
					}
				}

				m_modules[filepath.relative()] = std::move(module);
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmodule_service::unload_module(const core::fs::cfileinfo& filepath)
	{
		//- TODO: Before module is unloaded, post requests for unloading assets and resources

		if (const auto it = m_modules.find(filepath.relative()); it != m_modules.end())
		{
			m_modules.erase(it);
		}
	}

} //- io

RTTR_REGISTRATION
{
	using namespace io;

	rttr::cregistrator<cmodule_service, rttr::detail::as_raw_pointer>("cmodule_service")
		;
}