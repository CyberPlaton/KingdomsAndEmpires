#include "service_manager.hpp"
#include <plugin_logging.h>

namespace engine
{
// 	handle_type_t cservice_manager::s_service_count = 0;
// 	service_type_t cservice_manager::s_next_type = 0;
// 	umap_t<size_t, service_type_t > cservice_manager::s_service_types;
// 	array_t< ptr_t<cservice >, detail::iservice::C_SERVICE_COUNT_MAX> cservice_manager::s_services;

	//------------------------------------------------------------------------------------------------------------------------
	void cservice_manager::init(service_start_phase start_phase)
	{
		for (auto i = 0; i < s_service_count; ++i)
		{
			auto& service = s_services[i];
			if (service->phase() == start_phase && !service->on_start())
			{
				logging::log_error(fmt::format("Failed starting service '{}'", service->type()));
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cservice_manager::shutdown()
	{
		for (auto i = 0; i < s_service_count; ++i)
		{
			auto& service = s_services[i];
			service->on_shutdown();
			service.reset();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cservice_manager::on_update(float dt)
	{
		for (auto i = 0; i < s_service_count; ++i)
		{
			auto& service = s_services[i];
			service->on_update(dt);
		}
	}

} //- engine