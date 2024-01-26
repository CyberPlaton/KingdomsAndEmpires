#include "plugin_module_example_module.hpp"
#include <rttr/registration>

namespace module_example
{
	RTTR_PLUGIN_REGISTRATION
	{
		using namespace rttr;

		//- Registering component properties will make them seriailzable and retrievable with RTTR methods
		registration::class_<stargeting_component>("stargeting_component")
			.property("m_next_target", &stargeting_component::m_next_target)
			.property("m_cooldown", &stargeting_component::m_cooldown);

		registration::class_<cmy_system>("cmy_system")
			.constructor<flecs::world&>();

		registration::class_<cmy_module>("cmy_module")
			.constructor<flecs::world&>();

		ECS_MODULE(cmy_module);
	};

} //- module_example