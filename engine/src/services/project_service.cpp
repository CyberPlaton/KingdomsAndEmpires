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