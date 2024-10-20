#pragma once
#include <core.h>

namespace engine
{
	//- Service responsible for storing resource managers and returning an approppriate one for a given resource.
	//- Also allows for registering resource managers at any time.
	//- Note that the 'register_manager' function does not emplace the manager as a service! This should have been done already.
	//- On registering we merely store the type provided.
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
		bool register_manager();

		bool register_manager(const rttr::type& resource_type, const rttr::type& manager_type);

		template<typename TResource>
		core::cresource_manager<TResource>* manager();

		template<typename TClass>
		rttr::type manager_type() const;

	private:
		umap_t<rttr::type, rttr::type> m_managers;

		RTTR_ENABLE(core::cservice);

	private:
		template<typename TResource, typename TResourceManager>
		bool validate();
	};

	//- Retrieve type of the manager responsible for resource TClass
	//------------------------------------------------------------------------------------------------------------------------
	template<typename TClass>
	rttr::type engine::cresource_service::manager_type() const
	{
		static_assert(std::is_base_of_v<core::cresource, TClass>, "Invalid operation. TClass must be derived from core::cresource");

		return m_managers.at(rttr::type::get<TClass>());
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TResource, typename TResourceManager>
	bool engine::cresource_service::validate()
	{
		const auto& resource_type = rttr::type::get<TResource>();
		const auto& manager_type = rttr::type::get<TResourceManager>();

		//- Check that they are derived from required classes
		if (!resource_type.template is_derived_from<core::cresource>())
		{
			log_warn(fmt::format("Resource '{}' was not derived from 'core::cresource'",
				resource_type.get_name().data()));

			return false;
		}
		if (!manager_type.template is_derived_from<core::cresource_manager<TResource>>())
		{
			log_warn(fmt::format("Resource Manager '{}' was not derived from 'core::cresource_manager'",
				manager_type.get_name().data()));

			return false;
		}

		//- Check that required functions are implemented
		if (const auto& m = resource_type.get_method(core::cresource::C_DESTROY_FUNCTION_NAME.data());
			!m.is_valid() || !m.is_static())
		{
			log_warn(fmt::format("Resource '{}' did not define a static 'destroy' function",
				resource_type.get_name().data()));

			return false;
		}

		//- Check that the resource manager is already registered with the service manager
		if (const auto* service = core::cservice_manager::find(manager_type); !service)
		{
			log_warn(fmt::format("Resource Manager '{}' is not yet emplaced with 'core::cservice_manager', this might result in an issue!",
				manager_type.get_name().data()));

			//- Currently, we only issue a warning without failing...
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TResource>
	core::cresource_manager<TResource>* engine::cresource_service::manager()
	{
		if (const auto type = rttr::type::get<TResource>(); type.is_valid())
		{
			//- Get type of manager and find it
			const auto& manager_type = m_managers.at(type);

			if (auto* service = core::cservice_manager::find(manager_type); service)
			{
				return RCAST(core::cresource_manager<TResource>*, service);
			}
		}

		log_warn(fmt::format("Failed to find a manager for resource '{}'",
			rttr::type::get<TResource>().get_name().data()));

		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TResource, typename TResourceManager>
	bool engine::cresource_service::register_manager()
	{
		if (validate<TResource, TResourceManager>())
		{
			return register_manager(rttr::type::get<TResource>(), rttr::type::get<TResourceManager>());
		}
		return false;
	}


} //- engine