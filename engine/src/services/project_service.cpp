#include "project_service.hpp"

namespace engine
{
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
	ref_t<editor::cproject> cproject_service::create(stringview_t name)
	{
		auto h = algorithm::hash(name);

		if (m_projects.find(h) != m_projects.end())
		{
			return m_projects.at(h);
		}

		editor::cproject::sconfig cfg;
		cfg.m_basepath = "/";
		cfg.m_project_name = name.data();

		auto project = std::make_shared<editor::cproject>(std::move(cfg));
		m_projects[h] = project;
		return project;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cproject_service::set(const ref_t<editor::cproject>& project)
	{
		m_current = project;
	}

	//------------------------------------------------------------------------------------------------------------------------
	const editor::cproject& cproject_service::current() const
	{
		CORE_ASSERT(has(), "Invalid operation. Retrieving empty project!");

		return *m_current.get();
	}

	//------------------------------------------------------------------------------------------------------------------------
	editor::cproject& cproject_service::current()
	{
		CORE_ASSERT(has(), "Invalid operation. Retrieving empty project!");

		return *m_current.get();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cproject_service::load(stringview_t filepath)
	{
		m_current = std::make_shared<editor::cproject>(filepath); return has();
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