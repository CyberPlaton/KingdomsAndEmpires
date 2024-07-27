#pragma once
#include "ecs_world_context_holder.hpp"
#include "ecs_system.hpp"
#include "ecs_component.hpp"

namespace ecs
{
	//- information about an active module,
	//- dependencies can be traversed recursive
	struct smodule_info
	{
		std::string m_name;
		flecs::entity m_module_entity;
		vector_t<smodule_info> m_dependencies;

		vector_t<flecs::system> m_systems;
		vector_t<std::string> m_components;

		RTTR_ENABLE();
	};

	//- class to be inherited from a new module. Can define components, systems and other module dependencies.
	//- Declaration and import into ecs world will be done on mm().import_module<>() call.
	//- Note that ENGINE_API is intentionally skipped here.
	//-------------------------------------------------------------------------------------------------------------------------
	class imodule : public iworld_context_holder
	{
	public:
		imodule(flecs::world& w) : iworld_context_holder(w) {}
		virtual ~imodule() = default;

		template<class TModule>
		TModule& begin(stringview_t name);

		template<class TModule, class TDependency>
		TModule& depends_on();

		template<class TModule, class TSystem>
		TModule& subsystem();

		template<class TModule, class TComponent>
		TModule& comp();

		template<class TModule>
		bool end();

		const smodule_info& info() const { return m_info; }
		flecs::entity module() const{ return m_info.m_module_entity; }

	private:
		smodule_info m_info;

		RTTR_ENABLE(iworld_context_holder);
		RTTR_REGISTRATION_FRIEND;
	};

	//-------------------------------------------------------------------------------------------------------------------------
	template<class TModule>
	TModule& ecs::imodule::begin(stringview_t name)
	{
		m_info.m_name = name;
		m_info.m_module_entity = world().module<TModule>(name.data());

		return *reinterpret_cast<TModule*>(this);
	}

	//-------------------------------------------------------------------------------------------------------------------------
	template<class TModule, class TSystem>
	TModule& ecs::imodule::subsystem()
	{
		//- create and register system into current world
		TSystem sys(world());

		m_info.m_systems.emplace_back(sys.self());

		return *reinterpret_cast<TModule*>(this);
	}

	//-------------------------------------------------------------------------------------------------------------------------
	template<class TModule, class TDependency>
	TModule& ecs::imodule::depends_on()
	{
		//- create and import the dependency module first
		TDependency dep(world());

		//- store its information for later
		m_info.m_dependencies.push_back(dep.info());

		return *reinterpret_cast<TModule*>(this);
	}

	//-------------------------------------------------------------------------------------------------------------------------
	template<class TModule, class TComponent>
	TModule& ecs::imodule::comp()
	{
		world().component<TComponent>();

		m_info.m_components.emplace_back(rttr::type::get<TComponent>().get_name().data());

		return *reinterpret_cast<TModule*>(this);
	}

	//-------------------------------------------------------------------------------------------------------------------------
	template<class TModule>
	bool ecs::imodule::end()
	{
		world().import<TModule>();

		auto success = m_info.m_module_entity.is_valid();

		if (success)
		{
			log_info(fmt::format("Successfully imported module '{}'", typeid(TModule).name()));
		}
		else
		{
			log_error(fmt::format("Failed importing module '{}'", typeid(TModule).name()));
		}

		return success;
	}

} //- ecs