#include "animation_module.hpp"

namespace render_system
{

} //- render_system

RTTR_REGISTRATION
{
	using namespace render_system;

rttr::registration::class_<canimation_module>("canimation_module")
.constructor<flecs::world&>()
(
	rttr::policy::ctor::as_raw_ptr
	)
	;

}