#pragma once
#include "ecs_world_context_holder.hpp"
#include "ecs_system.hpp"

namespace ecs
{
	//- utility class simplifying registering a module into ecs
	//-------------------------------------------------------------------------------------------------------------------------
	class ENGINE_API imodule : public iworld_context_holder
	{
	public:
		struct smodule_info
		{
			string_t m_name;
			flecs::entity m_module_entity;
			vector_t<smodule_info> m_dependencies;
		};


		imodule(flecs::world& w);
		virtual ~imodule() = default;

		template<class TModuleType>
		imodule* begin(stringview_t name)
		{
			static_assert(std::is_base_of<imodule, TModuleType>::value, "TModuleType must be derived from imodule");

			m_info.m_name = name;
			m_info.m_module_entity = world().module<TModuleType>(name);
			
			return this;
		}

		template<class TDependency>
		imodule* depends_on()
		{
			static_assert(std::is_base_of<imodule, TDependency>::value, "TDependency must be derived from imodule");

			//- create and import the dependency module first
			TDependency dep(world());

			//- store its information for later
			m_info.m_dependencies.push_back(dep.info());

			return this;
		}

		template<class TSystem>
		imodule* subsystem()
		{
			static_assert(std::is_base_of<csystem, TSystem>::value, "TSystem must be derived from csystem");

			//- create and register system into current world
			TSystem sys(world());

			return this;
		}

		template<class TComponent>
		imodule* comp()
		{
			static_assert(std::is_base_of<icomponent, TComponent>::value, "TComponent must be derived from icomponent");

			world().component<TComponent>();
			return this;
		}

		template<class TModuleType>
		bool end()
		{
			static_assert(std::is_base_of<imodule, TModuleType>::value, "TModuleType must be derived from imodule");

			world().import<TModuleType>();

			auto success = m_info.m_module_entity.is_valid();
			if(success)
			{
				logging::log_info(fmt::format("Successfully imported module '{}'", typeid(TModuleType).name()));
			}
			else
			{
				logging::log_error(fmt::format("Failed importing module '{}'", typeid(TModuleType).name()));
			}
			return success;
		}

		inline const smodule_info& info() const { return m_info; }

		flecs::entity module() const;

	private:
		smodule_info m_info;
	};

} //- ecs