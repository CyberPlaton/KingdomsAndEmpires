#include "ecs_module.hpp"
#include "ecs_component.hpp"
#include "ecs_world_manager.hpp"

namespace ecs
{
	namespace modules
	{
		//------------------------------------------------------------------------------------------------------------------------
		void import_module(const sconfig& cfg)
		{
			//- If not done already, import dependency modules
			for (const auto& m : cfg.m_modules)
			{
				if (const auto type = rttr::type::get_by_name(m.data()); type.is_valid())
				{
					//- Calling module constructor that does the actual importing of the module
					type.create({ cfg.m_world });
				}
			}

			//- Registering components
			for (const auto& c : cfg.m_components)
			{
				if (const auto type = rttr::type::get_by_name(c.data()); type.is_valid())
				{
					//- Calling special component constructor that register the component to provided world
					type.create({ cfg.m_world });
				}
			}

			//- Registering systems
			for (const auto& s : cfg.m_systems)
			{
				if (const auto type = rttr::type::get_by_name(s.data()); type.is_valid())
				{
					//- Calling system constructor that does the actual registration of the system
					type.create({ cfg.m_world });
				}
			}
		}

	} //- modules

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