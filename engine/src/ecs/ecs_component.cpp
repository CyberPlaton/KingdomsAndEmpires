#include "ecs_component.hpp"

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	void sidentifier::show_ui(flecs::entity e)
	{
		auto* identifier = e.get_mut<sidentifier>();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void stransform::show_ui(flecs::entity e)
	{
		auto* transform = e.get_mut<stransform>();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void shierarchy::show_ui(flecs::entity e)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void sanimation::show_ui(flecs::entity e)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void ssprite::show_ui(flecs::entity e)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void scamera::show_ui(flecs::entity e)
	{

	}

} //- ecs

RTTR_REGISTRATION
{
	using namespace ecs;
	using namespace ecs::tag;

	rttr::cregistrator<sinvisible>("sinvisible")
		;


	rttr::ccomponent<shierarchy>("shierarchy")
		.prop("m_parent", &shierarchy::m_parent)
		.prop("m_children", &shierarchy::m_children)
		.meth(detail::C_COMPONENT_SHOW_UI_FUNC_NAME, &shierarchy::show_ui)
		;

	rttr::ccomponent<scamera>("scamera")
		.prop("m_position", &scamera::m_position)
		.prop("m_offset", &scamera::m_offset)
		.prop("m_zoom", &scamera::m_zoom)
		.prop("m_rotation", &scamera::m_rotation)
		.prop("m_viewrect", &scamera::m_viewrect)
		.prop("m_active", &scamera::m_active)
		.prop("m_renderlayer", &scamera::m_renderlayer)
		.meth(detail::C_COMPONENT_SHOW_UI_FUNC_NAME, &scamera::show_ui)
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
		.meth(detail::C_COMPONENT_SHOW_UI_FUNC_NAME, &sanimation::show_ui)
		;

	rttr::ccomponent<ssprite>("ssprite")
		.prop("m_flags", &ssprite::m_flags)
		.prop("m_layer", &ssprite::m_layer)
		.prop("m_source_rectangle", &ssprite::m_source_rectangle)
		.prop("m_tint", &ssprite::m_tint)
		.meth(detail::C_COMPONENT_SHOW_UI_FUNC_NAME, &ssprite::show_ui)
		;

	rttr::ccomponent<stransform>("stransform")
		.prop("m_position", &stransform::m_position)
		.prop("m_size", &stransform::m_size)
		.prop("m_angles", &stransform::m_angles)
		.meth(detail::C_COMPONENT_SHOW_UI_FUNC_NAME, &stransform::show_ui)
		;

	rttr::ccomponent<sidentifier>("sidentifier")
		.prop("m_uuid", &sidentifier::m_uuid)
		.prop("m_name", &sidentifier::m_name)
		.meth(detail::C_COMPONENT_SHOW_UI_FUNC_NAME, &sidentifier::show_ui)
		;

	rttr::registration::class_<icomponent>("icomponent")
		.method("name", &icomponent::name)
		;
}