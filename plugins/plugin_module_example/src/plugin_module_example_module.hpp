#pragma once
#include <plugin_module_example.h>
#include "components/example_components.hpp"
#include "systems/example_system.hpp"

namespace module_example
{
	//- Example module
	//------------------------------------------------------------------------------------------------------------------------
	class EXAMPLE_API cmy_module : public ecs::imodule<cmy_module>
	{
	public:
		cmy_module(flecs::world& w) : ecs::imodule<cmy_module>(w)
		{
			begin()
				.component<stargeting_component>()
				.subsystem<cmy_system>()
			.end();
		};
	};

} //- module_example