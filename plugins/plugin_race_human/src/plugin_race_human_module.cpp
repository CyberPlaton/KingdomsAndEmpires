#include "plugin_race_human_module.hpp"

namespace races
{

} //- races

RTTR_REGISTRATION
{
	using namespace races;

rttr::registration::class_<shuman>("shuman")
.method("serialize", &shuman::serialize)
;

rttr::default_constructor<shuman>();

rttr::registration::class_<chuman_race_module>("chuman_race_module")
.constructor<flecs::world&>()
(
	rttr::policy::ctor::as_raw_ptr
	)
	;

}