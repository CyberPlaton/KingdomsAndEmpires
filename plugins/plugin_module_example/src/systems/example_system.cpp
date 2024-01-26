#include "example_system.hpp"

namespace module_example
{
	RTTR_PLUGIN_REGISTRATION
	{
		using namespace rttr;

		registration::class_<cmy_system>("cmy_system")
			.constructor<flecs::world&>();

	};

} //- module_example