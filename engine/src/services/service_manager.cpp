#include "service_manager.hpp"
#include <plugin_logging.h>

namespace engine
{
	namespace
	{
		//------------------------------------------------------------------------------------------------------------------------
		inline const ref_t<cservice>& get_base_service(const rttr::variant& var)
		{
			return var.get_value<ref_t<cservice>>();
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline ref_t<cservice>& get_base_service(rttr::variant& var)
		{
			return var.get_value<ref_t<cservice>>();
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	void cservice_manager::init()
	{
		for (auto i = 0; i < s_service_count; ++i)
		{
			auto& service = get_base_service(s_services[i]);

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
			auto& service = get_base_service(s_services[i]);

			service->on_shutdown();
			s_services[i].clear();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cservice_manager::on_update(float dt)
	{
		for (auto i = 0; i < s_service_count; ++i)
		{
			auto& service = get_base_service(s_services[i]);

			service->on_update(dt);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cservice_manager::emplace(rttr::type service_type)
	{
		if (service_type.is_valid())
		{
			auto var = service_type.create({});

			if (var.is_valid())
			{
				auto t = s_next_type++;
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
		return false;
	}

} //- engine