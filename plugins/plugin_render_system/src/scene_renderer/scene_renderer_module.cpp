#include "scene_renderer_module.hpp"

namespace render_system
{
} //- render_system

RTTR_REGISTRATION
{
	using namespace render_system;

	rttr::registration::class_<cscene_render_module>("cscene_render_module")
	.constructor<flecs::world&>()
	(
			rttr::policy::ctor::as_raw_ptr
	)
	;
}