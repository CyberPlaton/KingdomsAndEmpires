#include "plugin_module_example_module.hpp"

namespace module_example
{
	//------------------------------------------------------------------------------------------------------------------------
	RTTR_PLUGIN_REGISTRATION
	{
		ECS_MODULE(cmy_module);
	};

} //- module_example