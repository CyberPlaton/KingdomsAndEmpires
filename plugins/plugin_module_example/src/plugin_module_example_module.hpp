#pragma once
#include "plugin_module_example_common.hpp"
#include "components/example_components.hpp"
#include "systems/example_system.hpp"

namespace module_example
{
	//- Trying alternative registering modules and systems without requiring templates.
	//- Note that while we are registering our classes inside RTTR_PLUGIN_REGISTRATION
	//- we do not need to explicitly write EXAMPLE_API for exported classes
	//------------------------------------------------------------------------------------------------------------------------
	class EXAMPLE_API cmy_module : public ecs::imodule
	{
	public:
		cmy_module(flecs::world& w) : ecs::imodule(w)
		{
			begin<cmy_module>()
				->comp<stargeting_component>()
				->subsystem<cmy_system>()
			->end<cmy_module>();
		}
	};

	//------------------------------------------------------------------------------------------------------------------------
	class EXAMPLE_API cmy_second_module : public ecs::imodule
	{
	public:
		cmy_second_module(flecs::world& w) : ecs::imodule(w)
		{
			begin<cmy_second_module>()
				->depends_on<cmy_module>()
				->comp<stargeting_component>()
				->subsystem<cmy_system>()
			->end<cmy_second_module>();

			auto test = w.entity("Walther");

			test.set<stargeting_component>({});
		}
	};

} //- module_example