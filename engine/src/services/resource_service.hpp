#pragma once
#include <core.h>
#include <plugin_logging.h>

namespace engine
{
	//- Service responsible for storing resource managers and returning an approppriate one for a given resource.
	//- Also allows for registering resource managers at any time.
	//------------------------------------------------------------------------------------------------------------------------
	class cresource_service final : public core::cservice
	{
	public:
		cresource_service() = default;
		~cresource_service();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float) override final;

		template<typename TResource, typename TResourceManager>
		void register_resource_manager();

		template<typename TResource>
		core::cresource_manager<TResource>* manager();

	private:
		umap_t<rttr::type, rttr::type> m_resource_managers;

		RTTR_ENABLE(core::cservice);
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TResource>
	core::cresource_manager<TResource>* engine::cresource_service::manager()
	{
		if (const auto type = rttr::type::get<TResource>()..get_id(); type.is_valid())
		{
			auto* service = core::cservice_manager::find(type);

			return RCAST(core::cresource_manager*, service);
		}

		logging::log_warn(fmt::format("Failed to find a manager for resource '{}'",
			rttr::type::get<TResource>().get_name().data()));

		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TResource, typename TResourceManager>
	void engine::cresource_service::register_resource_manager()
	{
		if (const auto resource_type = rttr::type::get<TResource>();
			m_resource_managers.find(resource_type) == m_resource_managers.end())
		{
			m_resource_managers[resource_type] = rttr::type::get<TResourceManager>();
		}
		else
		{
			//- Not an error, yet still report that we tried to register duplicates.
			logging::log_warn(fmt::format("Trying to register duplicate resource manager for type '{}' as '{}'",
				rttr::type::get<TResource>().get_name().data(), rttr::type::get<TResourceManager>().get_name().data()));
		}
	}


} //- engine

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(cresource_service)
	{
		rttr::cregistrator<cresource_service, rttr::detail::as_raw_pointer>("cresource_service")
			;
	};

} //- engine