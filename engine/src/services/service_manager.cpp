#include "service_manager.hpp"
#include <plugin_logging.h>

namespace engine
{
	namespace
	{
		//------------------------------------------------------------------------------------------------------------------------
		const ref_t<cservice>& get_base_service(const rttr::variant& var)
		{
			return var.get_value<ref_t<cservice>>();
		}

		//------------------------------------------------------------------------------------------------------------------------
		ref_t<cservice>& get_base_service(rttr::variant& var)
		{
			return var.get_value<ref_t<cservice>>();
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	void cservice_manager::init()
	{
		for (auto i = 0; i < s_service_count; ++i)
		{
			auto& var = s_services[i];

			auto& service = get_base_service(var);

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

			auto& service = get_base_service(var);

			service->on_shutdown();
			var.clear();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cservice_manager::on_update(float dt)
	{
		for (auto i = 0; i < s_service_count; ++i)
		{
			auto& var = s_services[i];

			auto& service = get_base_service(var);

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
		}
		return false;
	}

} //- engine