#include "service_manager.hpp"
#include <plugin_logging.h>

namespace engine
{
// 	handle_type_t cservice_manager::s_service_count = 0;
// 	service_type_t cservice_manager::s_next_type = 0;
// 	umap_t<size_t, service_type_t > cservice_manager::s_service_types;
// 	array_t< ptr_t<cservice >, detail::iservice::C_SERVICE_COUNT_MAX> cservice_manager::s_services;

	//------------------------------------------------------------------------------------------------------------------------
	void cservice_manager::init()
	{
		for (auto i = 0; i < s_service_count; ++i)
		{
			auto& var = s_services[i];

			auto& service = var.get_value<ref_t<cservice>>();

			if (!service->on_start())
			{
				logging::log_error(fmt::format("Failed starting service '{}'", var.get_type().get_name().data()));
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cservice_manager::shutdown()
	{
		for (auto i = 0; i < s_service_count; ++i)
		{
			auto& var = s_services[i];
			auto& service = var.get_value<cservice>();

			service.on_shutdown();
			var.clear();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cservice_manager::on_update(float dt)
	{
		for (auto i = 0; i < s_service_count; ++i)
		{
			auto& var = s_services[i];
			auto& service = var.get_value<cservice>();

			service.on_update(dt);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cservice_manager::emplace(rttr::type service_type)
	{
		if (service_type.is_valid())
		{
			auto id = SCAST(service_type_t, service_type.get_id());

			auto var = service_type.create({});

			if (var.is_valid())
			{
				auto t = s_next_type++;
				s_services[t] = std::move(var);
				s_service_count++;

				return true;
			}
		}
		return false;
	}

} //- engine