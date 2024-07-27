#include "plugin_module_example_module.hpp"

namespace module_example
{

} //- module_example

RTTR_REGISTRATION
{
	using namespace module_example;

rttr::registration::class_<cmy_module>("cmy_module")
.constructor<flecs::world&>()
(
	rttr::policy::ctor::as_raw_ptr
	)
	;

rttr::registration::class_<cmy_second_module>("cmy_second_module")
.constructor<flecs::world&>()
(
	rttr::policy::ctor::as_raw_ptr
	)
	;

rttr::registration::class_<cmy_third_module>("cmy_third_module")
.constructor<flecs::world&>()
(
	rttr::policy::ctor::as_raw_ptr
	)
	;

}