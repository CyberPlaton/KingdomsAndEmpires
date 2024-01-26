#include "ecs_component.hpp"

namespace ecs
{
	RTTR_PLUGIN_REGISTRATION
	{
		using namespace rttr;

		registration::class_<icomponent>("icomponent");

	};

} //- ecs