#include "plugin_module_example_module.hpp"
#include <rttr/registration>

namespace module_example
{
	RTTR_PLUGIN_REGISTRATION
	{
		using namespace rttr;

// 		registration::class_<cmy_module>("cmy_module")
// 			.constructor<flecs::world&>();
// 
// 		ECS_MODULE(cmy_module);

		registration::class_<cmy_module>("cmy_module")
			.constructor<flecs::world&>();

		registration::class_<cmy_second_module>("cmy_second_module")
			.constructor<flecs::world&>();

	};

} //- module_example