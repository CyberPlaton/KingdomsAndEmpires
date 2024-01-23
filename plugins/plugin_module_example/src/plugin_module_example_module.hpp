#pragma once
#include <plugin_module_example.h>
#include "components/example_components.hpp"
#include "systems/example_system.hpp"

namespace module_example
{
	class cexample_module : public ecs::imodule<cexample_module>
	{
	public:
		cexample_module() : ecs::imodule<cexample_module>("cexample_module")
		{
		};
	};

} //- module_example