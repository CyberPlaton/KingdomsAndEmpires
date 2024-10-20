#include "ecs.hpp"

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	ecs::cworld& world()
	{
		return ecs::cworld_manager::instance().active();
	}

} //- ecs

RTTR_REGISTRATION
{
	using namespace ecs;

	//- Section: ecs components
	//------------------------------------------------------------------------------------------------------------------------
	rttr::cregistrator<tag::sinvisible>("sinvisible")
		;

	rttr::ccomponent<shierarchy>("shierarchy")
		.prop("m_parent", &shierarchy::m_parent)
		.prop("m_children", &shierarchy::m_children)
		.meth(ecs::detail::C_COMPONENT_SHOW_UI_FUNC_NAME, &shierarchy::show_ui)
		.meth(ecs::detail::C_COMPONENT_ADD_FUNC_NAME, &shierarchy::add)
		.meth(ecs::detail::C_COMPONENT_REMOVE_FUNC_NAME, &shierarchy::remove)
		;

	rttr::ccomponent<scamera>("scamera")
		.prop("m_position", &scamera::m_position)
		.prop("m_offset", &scamera::m_offset)
		.prop("m_zoom", &scamera::m_zoom)
		.prop("m_rotation", &scamera::m_rotation)
		.prop("m_viewrect", &scamera::m_viewrect)
		.prop("m_active", &scamera::m_active)
		.prop("m_renderlayer", &scamera::m_renderlayer)
		.meth(ecs::detail::C_COMPONENT_SHOW_UI_FUNC_NAME, &scamera::show_ui)
		.meth(ecs::detail::C_COMPONENT_ADD_FUNC_NAME, &scamera::add)
		.meth(ecs::detail::C_COMPONENT_REMOVE_FUNC_NAME, &scamera::remove)
		;

	rttr::ccomponent<sanimation>("sanimation")
		.prop("m_animation", &sanimation::m_animation)
		.prop("m_animation_state", &sanimation::m_animation_state)
		.prop("m_animation_state_changed", &sanimation::m_animation_state_changed)
		.prop("m_current_keyframe", &sanimation::m_current_keyframe)
		.prop("m_current_keyframe_index", &sanimation::m_current_keyframe_index)
		.prop("m_cursor", &sanimation::m_cursor)
		.prop("m_duration", &sanimation::m_duration)
		.prop("m_frame_countx", &sanimation::m_frame_countx)
		.prop("m_frame_county", &sanimation::m_frame_county)
		.prop("m_frame_height", &sanimation::m_frame_height)
		.prop("m_frame_width", &sanimation::m_frame_width)
		.prop("m_keyframes", &sanimation::m_keyframes)
		.meth(ecs::detail::C_COMPONENT_SHOW_UI_FUNC_NAME, &sanimation::show_ui)
		.meth(ecs::detail::C_COMPONENT_ADD_FUNC_NAME, &sanimation::add)
		.meth(ecs::detail::C_COMPONENT_REMOVE_FUNC_NAME, &sanimation::remove)
		;

	rttr::ccomponent<smaterial>("smaterial")
		.prop("m_renderstate", &smaterial::m_renderstate)
		.prop("m_texture", &smaterial::m_texture)
		.prop("m_program", &smaterial::m_program)
		.prop("m_flags", &smaterial::m_flags)
		.meth(ecs::detail::C_COMPONENT_SHOW_UI_FUNC_NAME, &smaterial::show_ui)
		.meth(ecs::detail::C_COMPONENT_ADD_FUNC_NAME, &smaterial::add)
		.meth(ecs::detail::C_COMPONENT_REMOVE_FUNC_NAME, &smaterial::remove)
		;

	rttr::ccomponent<ssprite_renderer>("ssprite_renderer")
		.prop("m_source_rect", &ssprite_renderer::m_source_rect)
		.prop("m_tint", &ssprite_renderer::m_tint)
		.prop("m_layer", &ssprite_renderer::m_layer)
		.meth(ecs::detail::C_COMPONENT_SHOW_UI_FUNC_NAME, &ssprite_renderer::show_ui)
		.meth(ecs::detail::C_COMPONENT_ADD_FUNC_NAME, &ssprite_renderer::add)
		.meth(ecs::detail::C_COMPONENT_REMOVE_FUNC_NAME, &ssprite_renderer::remove)
		;

	rttr::ccomponent<stransform>("stransform")
		.prop("m_position", &stransform::m_position)
		.prop("m_scale", &stransform::m_scale)
		.prop("m_shear", &stransform::m_shear)
		.prop("m_rotation", &stransform::m_rotation)
		.meth(ecs::detail::C_COMPONENT_SHOW_UI_FUNC_NAME, &stransform::show_ui)
		.meth(ecs::detail::C_COMPONENT_ADD_FUNC_NAME, &stransform::add)
		.meth(ecs::detail::C_COMPONENT_REMOVE_FUNC_NAME, &stransform::remove)
		;

	rttr::ccomponent<sidentifier>("sidentifier")
		.prop("m_uuid", &sidentifier::m_uuid)
		.prop("m_name", &sidentifier::m_name)
		.meth(ecs::detail::C_COMPONENT_SHOW_UI_FUNC_NAME, &sidentifier::show_ui)
		.meth(ecs::detail::C_COMPONENT_ADD_FUNC_NAME, &sidentifier::add)
		.meth(ecs::detail::C_COMPONENT_REMOVE_FUNC_NAME, &sidentifier::remove)
		;

	rttr::registration::class_<icomponent>("icomponent")
		.method("name", &icomponent::name)
		;

	//- Section: ecs modules
	//------------------------------------------------------------------------------------------------------------------------
	rttr::cregistrator<modules::sconfig, rttr::detail::as_object>("sconfig")
		.ctor<flecs::world&>()
		.prop("m_name",			&modules::sconfig::m_name)
		.prop("m_components",	&modules::sconfig::m_components)
		.prop("m_systems",		&modules::sconfig::m_systems)
		.prop("m_modules",		&modules::sconfig::m_modules)
		;
}