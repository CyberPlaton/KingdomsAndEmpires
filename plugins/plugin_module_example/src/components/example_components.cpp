#include "example_components.hpp"
#include <rttr/registration>

namespace module_example
{
	RTTR_PLUGIN_REGISTRATION
	{
		using namespace rttr;

		registration::class_<stargeting_component>("stargeting_component")
			.property("m_next_target", &stargeting_component::m_next_target)
			.property("m_cooldown", &stargeting_component::m_cooldown);

	};

} //- module_example