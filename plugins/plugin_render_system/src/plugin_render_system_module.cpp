#include "plugin_render_system_module.hpp"

RTTR_PLUGIN_REGISTRATION
{
	using namespace render_system;

	rttr::cregistrator<srender_module>("srender_module")
		.ctor<flecs::world&>()
		;
	
	rttr::cregistrator<srender_system>("srender_system")
		.ctor<flecs::world&>()
		;
	
	rttr::cregistrator<sdebug_render_system>("sdebug_render_system")
		.ctor<flecs::world&>()
		;

	rttr::cregistrator<sanimation_module>("sanimation_module")
		.ctor<flecs::world&>()
		;

	rttr::cregistrator<sanimation_system>("sanimation_system")
		.ctor<flecs::world&>()
		;
};

namespace render_system
{

} //- render_system