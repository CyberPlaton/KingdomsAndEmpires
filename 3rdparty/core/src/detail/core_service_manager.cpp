#include "core_service_manager.hpp"

namespace core
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
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(logging_verbosity_error,
						fmt::format("Failed starting service '{}'", s_services[i].get_type().get_name().data()));
				}
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
	core::cservice* cservice_manager::find(const rttr::type& type)
	{
		auto id = type.get_id();

		if (s_service_types.find(id) != s_service_types.end())
		{
			auto& var = s_services[s_service_types[id]];

			ASSERT(var.is_valid(), "Invalid operation. Trying to retrieve a released service");

			return get_base_service(var);
		}

		ASSERT(false, "Invalid operation. Service does not exist");

		return nullptr;
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
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(logging_verbosity_error,
						fmt::format("\tFailed constructing service '{}'. Did you register a default constructor?",
							service_type.get_name().data()));
				}
			}
		}
		else
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(logging_verbosity_error,
					fmt::format("\tCan not emplace service '{}'. Maximum number of services reached, increase value in 'core' library", service_type.get_name().data()));
			}
		}
		return false;
	}

} //- core