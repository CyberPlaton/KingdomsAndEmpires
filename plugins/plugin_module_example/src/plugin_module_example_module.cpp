#include "plugin_module_example_module.hpp"

RTTR_PLUGIN_REGISTRATION
{
	using namespace rttr;

	//- Registering component properties will make them seriailzable and retrievable with RTTR methods
	registration::class_<module_example::stargeting_component>("module_example::stargeting_component")
		.property("m_next_target", &module_example::stargeting_component::m_next_target)
		.property("m_cooldown", &module_example::stargeting_component::m_cooldown);

	registration::class_<module_example::cmy_system>("module_example::cmy_system")
		.constructor<flecs::world&>();

	registration::class_<module_example::cmy_module>("module_example::cmy_module")
		.constructor<flecs::world&>();

	ECS_MODULE(module_example::cmy_module);
};

namespace module_example
{

} //- module_example