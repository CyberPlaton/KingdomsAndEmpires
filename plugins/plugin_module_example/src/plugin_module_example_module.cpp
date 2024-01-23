#include "plugin_module_example_module.hpp"

namespace module_example
{
	RTTR_PLUGIN_REGISTRATION
	{
		cexample_module::begin()
			.depends_on("plugin_physics::cdestruction_module")
			.system(targeting_system);

		cmodule module;
		module.system(targeting_system);
	};

} //- module_example