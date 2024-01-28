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
		cmy_module(ref_t<flecs::world> w) :
			imodule(w)
		{
			begin<cmy_module>()
				->comp<stargeting_component>()
				->subsystem<cmy_system>()
			->end();
		}
	};

	//------------------------------------------------------------------------------------------------------------------------
	class EXAMPLE_API cmy_second_module : public ecs::imodule
	{
	public:
		cmy_second_module(ref_t<flecs::world> w) :
			imodule(w)
		{
			begin<cmy_second_module>()
				->comp<stargeting_component>()
				->subsystem<cmy_system>()
			->end();
		}
	};

} //- module_example