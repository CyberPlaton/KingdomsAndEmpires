#include "plugin_module_example_module.hpp"

//------------------------------------------------------------------------------------------------------------------------
RTTR_PLUGIN_REGISTRATION
{
	using namespace rttr;

	registration::class_<module_example::imodule>("module_example::imodule")
		.constructor<flecs::world&>();

	registration::class_<module_example::cmy_module>("module_example::cmy_module")
		.constructor<flecs::world&>();

	//registration::class_<cmy_module>("cmy_module")
	//	.constructor<flecs::world&>();

	//ECS_MODULE(cmy_module);
};

namespace module_example
{

} //- module_example