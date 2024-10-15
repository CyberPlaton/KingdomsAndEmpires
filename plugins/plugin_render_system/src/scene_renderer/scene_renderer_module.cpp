#include "scene_renderer_module.hpp"

namespace render_system
{
	//------------------------------------------------------------------------------------------------------------------------
	void scene_render_system(float dt)
	{
		CORE_NAMED_ZONE("Render System");
	}

	//------------------------------------------------------------------------------------------------------------------------
	void scene_debug_render_system(float dt)
	{
		CORE_NAMED_ZONE("Debug Render System");
	}

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