#pragma once
#include "plugin_module_example_common.hpp"
#include "components/example_components.hpp"
#include "systems/example_system.hpp"

namespace module_example
{
	class EXAMPLE_API imodule
	{
	public:
		imodule(flecs::world& w) : m_world(&w) {}

		template<class TModuleType>
		imodule* begin()
		{
			world().module<TModuleType>();

			return this;
		}

		template<class TDependency>
		imodule* depends_on()
		{
			world().import<TDependency>();

			return this;
		}

		template<class TSystem>
		imodule* subsystem()
		{
			TSystem(world());

			return this;
		}

		template<class TComponent>
		imodule* comp()
		{
			world().component<TComponent>();

			return this;
		}

		bool end()
		{
			return true;
		}

	private:
		flecs::world* m_world;

	private:
		flecs::world& world() { ASSERT(m_world, "World for module was not set!"); return *m_world; }

		RTTR_ENABLE();
	};


	//- Trying alternative registering modules and systems without requiring templates.
	//- Note that while we are registering our classes inside RTTR_PLUGIN_REGISTRATION
	//- we do not need to explicitly write EXAMPLE_API for exported classes
	//------------------------------------------------------------------------------------------------------------------------
	class EXAMPLE_API cmy_module : public imodule
	{
	public:
		cmy_module(flecs::world& w) : imodule(w)
		{
			begin<cmy_module>()
				->comp<stargeting_component>()
				->subsystem<cmy_system>()
			->end();
		};

		RTTR_ENABLE(imodule);
	};

} //- module_example