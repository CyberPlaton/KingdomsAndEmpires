#pragma once
#include "ecs_world_context_holder.hpp"
#include "ecs_system.hpp"

namespace ecs
{
	//- utility class simplifying registering a module into ecs
	//-------------------------------------------------------------------------------------------------------------------------
	class imodule : public iworld_context_holder
	{
	public:
		imodule(flecs::world& /*w*/){}
		virtual ~imodule() = default;

		flecs::entity module() const;

		template<class TModuleType>
		imodule* begin()
		{
			static_assert(std::is_base_of<imodule, TModuleType>::value, "TModuleType must be derived from imodule");

			m_module = world().module<TModuleType>();
			return this;
		}

		template<class TDependency>
		imodule* depends_on()
		{
			static_assert(std::is_base_of<imodule, TDependency>::value, "TDependency must be derived from imodule");

			//- create and import the dependency module first
			TDependency dep(world());
			return this;
		}

		template<class TSystem>
		imodule* subsystem()
		{
			static_assert(std::is_base_of<csystem, TSystem>::value, "TSystem must be derived from csystem");

			//- create and register system into current world
			TSystem sys;
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

			auto success = m_module.is_valid();
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

	private:
		flecs::entity m_module;
	};

} //- ecs