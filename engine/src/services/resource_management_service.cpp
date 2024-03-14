#include "resource_management_service.hpp"
#include <plugin_logging.h>

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	bool cresource_management_service::on_start()
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------------------------------
	void cresource_management_service::on_shutdown()
	{

	}
	
	//------------------------------------------------------------------------------------------------------------------------
	void cresource_management_service::on_update(float dt)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cresource_management_service::emplace(rttr::type manager_type)
	{
		if (m_next_type < core::iresource_manager::C_MANAGER_COUNT_MAX && manager_type.is_valid())
		{
			auto var = manager_type.create({});

			if (var.is_valid())
			{
				auto id = manager_type.get_id();
				auto t = m_next_type++;
				m_manager_types[id] = t;
				m_managers[t] = std::move(var);
				++m_manager_count;

				return true;
			} 
			else
			{
				logging::log_error(fmt::format("\tFailed constructing resource manager '{}'. Did you register a default constructor?",
					manager_type.get_name().data()));
			}
		}
		else
		{
			logging::log_error(fmt::format("\tCan not emplace manager '{}'. Maximum number of managers reached, increase value in 'core' library",
				manager_type.get_name().data()));
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::iresource_manager* cresource_management_service::get_base_manager(rttr::variant& var)
	{
		return var.get_value<core::iresource_manager*>();
	}

} //- engine