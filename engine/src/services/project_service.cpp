#include "project_service.hpp"

namespace engine
{
	namespace
	{
		constexpr stringview_t C_PROJECT_EXT = "project";

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	cproject_service::~cproject_service()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cproject_service::on_start()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cproject_service::on_shutdown()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cproject_service::on_update(float dt)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	project_ref_t cproject_service::create_project(stringview_t basepath, stringview_t name)
	{
		const auto h = algorithm::hash(name);

		core::cscope_mutex m(m_mutex);

		//- Sanity checks
		if (!basepath.empty() &&
			!name.empty() &&
			!core::cfilesystem::find_file(basepath.data(), fmt::format("{}.{}", name.data(), C_PROJECT_EXT.data()).data()) &&
			core::cfilesystem::create_file_in(basepath.data(), name.data(), C_PROJECT_EXT.data()))
		{
			editor::cproject::sconfig cfg;
			cfg.m_project_name = fmt::format("{}.{}", name.data(), C_PROJECT_EXT.data()).data();
			cfg.m_basepath = basepath.data();

			const auto project = std::make_shared<editor::cproject>(std::move(cfg));

			m_projects[h] = project;

			if (const auto result = core::fs::save_text_to_file(fmt::format("{}/{}.{}", basepath.data(), name.data(), C_PROJECT_EXT.data()), core::io::to_json_string(*project.get()).data());
				result != core::file_io_status_success)
			{
				log_error(fmt::format("Could not save file for project '{}' at '{}'", name.data(), basepath.data()));
			}
			return project;
		}
		else if (m_projects.find(h) != m_projects.end())
		{
			return m_projects.at(h);
		}
		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cproject_service::open_project(const core::fs::cfileinfo& filepath)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cproject_service::remove_project(stringview_t name)
	{
		if (const auto it = m_projects.find(algorithm::hash(name)); it != m_projects.end())
		{
			m_projects.erase(it);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cproject_service::delete_project(const core::fs::cfileinfo& filepath)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cproject_service::set_current(stringview_t name)
	{
		if (const auto it = m_projects.find(algorithm::hash(name)); it != m_projects.end())
		{
			m_current = it->second;
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cproject_service::set_current(const core::fs::cfileinfo& filepath)
	{
		return set_current(filepath.stem());
	}

	//------------------------------------------------------------------------------------------------------------------------
	project_ref_t cproject_service::current() const
	{
		CORE_ASSERT(has(), "Invalid operation. No project was set!");

		return m_current;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cproject_service::has() const
	{
		return m_current.get() != nullptr;
	}

} //- engine

RTTR_REGISTRATION
{
	using namespace engine;

	rttr::cregistrator<cproject_service, rttr::detail::as_raw_pointer>("cproject_service")
		;
}