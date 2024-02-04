#pragma once
#include "ecs_world_context_holder.hpp"
#include "ecs_system.hpp"

namespace ecs
{
	//- information about an active module,
	//- dependencies can be traversed recursive
	struct smodule_info
	{
		string_t m_name;
		flecs::entity m_module_entity;
		vector_t<smodule_info> m_dependencies;
	};

	//- class to be inherited from a new module. Can define components, systems and other module dependencies.
	//- Declaration and import into ecs world will be done on mm().import_module<>() call.
	//- Note that ENGINE_API is intentionally skipped here.
	//-------------------------------------------------------------------------------------------------------------------------
	template<class TModule>
	class imodule : public iworld_context_holder
	{
	public:
		imodule(flecs::world& w) : iworld_context_holder(w) {}
		virtual ~imodule() = default;

		imodule<TModule>& begin(stringview_t name);

		template<class TDependency>
		imodule<TModule>& depends_on();

		template<class TSystem>
		imodule<TModule>& subsystem();

		template<class TComponent>
		imodule<TModule>& comp();

		bool end();

		inline const smodule_info& info() const { return m_info; }
		inline flecs::entity module() const{ return m_info.m_module_entity; }

	private:
		smodule_info m_info;
	};

	//-------------------------------------------------------------------------------------------------------------------------
	template<class TModule>
	imodule<TModule>& ecs::imodule<TModule>::begin(stringview_t name)
	{
		m_info.m_name = name;
		m_info.m_module_entity = world().module<TModule>(name);

		return *this;
	}

	//-------------------------------------------------------------------------------------------------------------------------
	template<class TModule>
	template<class TSystem>
	imodule<TModule>& ecs::imodule<TModule>::subsystem()
	{
		static_assert(std::is_base_of<csystem, TSystem>::value, "TSystem must be derived from csystem");

		//- create and register system into current world
		TSystem sys(world());

		return *this;
	}

	//-------------------------------------------------------------------------------------------------------------------------
	template<class TModule>
	template<class TDependency>
	imodule<TModule>& ecs::imodule<TModule>::depends_on()
	{
		//- create and import the dependency module first
		TDependency dep(world());

		//- store its information for later
		m_info.m_dependencies.push_back(dep.info());

		return *this;
	}

	//-------------------------------------------------------------------------------------------------------------------------
	template<class TModule>
	template<class TComponent>
	imodule<TModule>& ecs::imodule<TModule>::comp()
	{
		static_assert(std::is_base_of<icomponent, TComponent>::value, "TComponent must be derived from icomponent");

		world().component<TComponent>();

		return *this;
	}

	//-------------------------------------------------------------------------------------------------------------------------
	template<class TModule>
	bool ecs::imodule<TModule>::end()
	{
		world().import<TModule>();

		auto success = m_info.m_module_entity.is_valid();

		if (success)
		{
			logging::log_info(fmt::format("Successfully imported module '{}'", typeid(TModule).name()));
		}
		else
		{
			logging::log_error(fmt::format("Failed importing module '{}'", typeid(TModule).name()));
		}

		return success;
	}

} //- ecs