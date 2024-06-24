#include "resource_service.hpp"

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	cresource_service::~cresource_service()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cresource_service::on_start()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cresource_service::on_shutdown()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cresource_service::on_update(float)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cresource_service::register_manager(const rttr::type& resource_type, const rttr::type& manager_type)
	{
		if (m_managers.find(resource_type) == m_managers.end())
		{
			m_managers.emplace(resource_type, manager_type);
		}
		else
		{
			//- Not an error for release, yet still report that we tried to register duplicates and
			//- handle as error for debug.
			logging::log_warn(fmt::format("Trying to register duplicate resource manager for type '{}' as '{}'",
				resource_type.get_name().data(), manager_type.get_name().data()));
#if CORE_DEBUG
			return false;
#endif
		}
		return true;
	}

} //- engine