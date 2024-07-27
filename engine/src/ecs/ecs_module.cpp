#include "ecs_module.hpp"
#include "ecs_component.hpp"
#include "ecs_world_manager.hpp"
#include <plugin_logging.h>

namespace ecs
{
} //- ecs

RTTR_REGISTRATION
{
	using namespace ecs;

	rttr::registration::class_<smodule_info>("smodule_info")
	.property("m_name", &smodule_info::m_name)
	.property("m_module_entity", &smodule_info::m_module_entity)
	.property("m_dependencies", &smodule_info::m_dependencies)
	;

	rttr::registration::class_<imodule>("imodule")
	.constructor<flecs::world&>()
	(
		rttr::policy::ctor::as_raw_ptr
		)
		.property("m_info", &imodule::m_info)
		;
}