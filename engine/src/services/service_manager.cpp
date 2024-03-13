#include "service_manager.hpp"
#include <plugin_logging.h>

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	core::cservice* cservice_manager::get_base_service(rttr::variant& var)
	{
		return var.get_value<core::cservice*>();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cservice_manager::init()
	{
		for (auto i = 0; i < s_service_count; ++i)
		{
			auto* service = get_base_service(s_services[i]);

			if (!service->on_start())
			{
				logging::log_error(fmt::format("Failed starting service '{}'", s_services[i].get_type().get_name().data()));
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cservice_manager::shutdown()
	{
		for (auto i = 0; i < s_service_count; ++i)
		{
			auto* service = get_base_service(s_services[i]);

			service->on_shutdown();

			//- delete underlying raw pointers
			auto type = s_services[i].get_type();

			if (type.is_pointer())
			{
				type.destroy(s_services[i]);
			}

			s_services[i].clear();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cservice_manager::on_update(float dt)
	{
		for (auto i = 0; i < s_service_count; ++i)
		{
			auto* service = get_base_service(s_services[i]);

			service->on_update(dt);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cservice_manager::emplace(rttr::type service_type)
	{
		if (s_next_type < core::cservice::C_SERVICE_COUNT_MAX && service_type.is_valid())
		{
			auto var = service_type.create({});

			if (var.is_valid())
			{
				auto id = service_type.get_id();
				auto t = s_next_type++;
				s_service_types[id] = t;
				s_services[t] = std::move(var);
				s_service_count++;

				return true;
			} 
			else
			{
				logging::log_error(fmt::format("\tFailed constructing service '{}'. Did you register a default constructor?",
					service_type.get_name().data()));
			}
		}
		else
		{
			logging::log_error(fmt::format("\tCan not emplace service '{}'. Maximum number of services reached, increase value in 'core' library",
				service_type.get_name().data()));
		}
		return false;
	}

} //- engine