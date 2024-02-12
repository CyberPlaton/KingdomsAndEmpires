#pragma once
#include <core.h>
#include <flecs.h>

//- use this macro for defining a simple component. Simple means that
//- it does not inherit from a complex component hierarchy.
#define DECLARE_COMPONENT(c) \
static stringview_t name() { static constexpr stringview_t C_NAME = STRING(c); return C_NAME; } \
static void serialize(flecs::entity e, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) \
{ \
	const auto* comp = e.get<c>(); \
	writer.StartObject(); \
	writer.String(name(), strlen(name()), false); \
	writer.String(io::to_json(comp).c_str()); \
	writer.EndObject(); \
}

namespace ecs
{
	//- base class for all components
	//------------------------------------------------------------------------------------------------------------------------
	struct icomponent
	{
		static stringview_t name() { static constexpr stringview_t C_NAME = "icomponent"; return C_NAME; };

		RTTR_ENABLE();
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sidentifier final : public icomponent
	{
		DECLARE_COMPONENT(sidentifier);

		std::string get_uuid() const { return m_self.name(); }
		void set_uuid(std::string uuid) { m_self.set_name(uuid.c_str());}

		flecs::entity m_self;

		//- internal usage
		entity_proxy_t m_aabb_proxy		= 0;
		unsigned m_aabb_proxy_query_key = 0;

		RTTR_ENABLE(icomponent);
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct stransform final : public icomponent
	{
		DECLARE_COMPONENT(stransform);

		float m_x = 0.0f, m_y = 0.0f, m_w = 0.0f, m_h = 0.0f, m_rotation = 0.0f;

		RTTR_ENABLE(icomponent);
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct ssprite final : public icomponent
	{
		DECLARE_COMPONENT(ssprite);

		vector_t<core::spair<texture_t, material_t>> m_materials;

		//- represents a rectangle in actual texture pixel size
		unsigned short m_sourcex= 0;
		unsigned short m_sourcey= 0;
		unsigned short m_sourcew= 0;
		unsigned short m_sourceh= 0;
		float m_originx			= 0.0f;
		float m_originy			= 0.0f;
		core::scolor m_tint		= core::scolor(core::common_color_neutral1000);
		bool m_visible			= true;
		bool m_flipx			= false;
		bool m_flipy			= false;
		unsigned char m_layer	= 0;

		RTTR_ENABLE(icomponent);
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sanimation final : public icomponent
	{
		DECLARE_COMPONENT(sanimation);

		using keyframe_t = core::spair<unsigned char, unsigned char>;

		vector_t<keyframe_t> m_keyframes;
		keyframe_t m_current_keyframe;

		float m_cursor							= 0.0f;
		float m_duration						= 0.0f;

		unsigned m_animation					= 0;
		unsigned m_animation_state				= 0;

		unsigned short m_frame_countx			= 0;
		unsigned short m_frame_county			= 0;
		unsigned short m_frame_width			= 0;
		unsigned short m_frame_height			= 0;

		unsigned char m_current_keyframe_index	= 0;
		bool m_animation_state_changed			= false;

		RTTR_ENABLE(icomponent);
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct shierarchy final : public icomponent
	{
		DECLARE_COMPONENT(shierarchy);

		core::cuuid m_parent;
		vector_t<core::cuuid> m_children;

		RTTR_ENABLE(icomponent);
	};

} //- ecs

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(sidentifier)
	{
		rttr::registration::class_<sidentifier>("sidentifier")
			.property("m_uuid", &sidentifier::get_uuid, &sidentifier::set_uuid)
			.method("serialize", &sidentifier::serialize)
			;
	};

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(stransform)
	{
		rttr::registration::class_<stransform>("stransform")
			.property("m_x", &stransform::m_x)
			.property("m_y", &stransform::m_y)
			.property("m_w", &stransform::m_w)
			.property("m_h", &stransform::m_h)
			.property("m_rotation", &stransform::m_rotation)
			.method("serialize", &stransform::serialize)
			;
	};

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(ssprite)
	{
		rttr::registration::class_<ssprite>("ssprite")
			.property("m_materials", &ssprite::m_materials)
			.property("m_flipx", &ssprite::m_flipx)
			.property("m_flipy", &ssprite::m_flipy)
			.property("m_layer", &ssprite::m_layer)
			.property("m_originx", &ssprite::m_originx)
			.property("m_originy", &ssprite::m_originy)
			.property("m_sourceh", &ssprite::m_sourceh)
			.property("m_sourcew", &ssprite::m_sourcew)
			.property("m_sourcex", &ssprite::m_sourcex)
			.property("m_sourcey", &ssprite::m_sourcey)
			.property("m_tint", &ssprite::m_tint)
			.method("serialize", &ssprite::serialize)
			;
	};

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(sanimation)
	{
		rttr::registration::class_<sanimation>("sanimation")
			.property("m_animation", &sanimation::m_animation)
			.property("m_animation_state", &sanimation::m_animation_state)
			.property("m_animation_state_changed", &sanimation::m_animation_state_changed)
			.property("m_current_keyframe", &sanimation::m_current_keyframe)
			.property("m_current_keyframe_index", &sanimation::m_current_keyframe_index)
			.property("m_cursor", &sanimation::m_cursor)
			.property("m_duration", &sanimation::m_duration)
			.property("m_frame_countx", &sanimation::m_frame_countx)
			.property("m_frame_county", &sanimation::m_frame_county)
			.property("m_frame_height", &sanimation::m_frame_height)
			.property("m_frame_width", &sanimation::m_frame_width)
			.property("m_keyframes", &sanimation::m_keyframes)
			.method("serialize", &sanimation::serialize)
			;
	};

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(shierarchy)
	{
		rttr::registration::class_<shierarchy>("shierarchy")
			.property("m_parent", &shierarchy::m_parent)
			.property("m_children", &shierarchy::m_children)
			.method("serialize", &shierarchy::serialize)
			;
	};

} //- ecs