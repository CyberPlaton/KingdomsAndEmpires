#pragma once
#include <core.h>
#include <flecs.h>

namespace ecs
{
	namespace meta
	{
		constexpr std::string_view C_COMPONENT_NOT_EDITABLE = "NOT_EDITABLE";
		constexpr std::string_view C_COMPONENT_NOT_VISIBLE	= "NOT_VISIBLE";

	} //- meta

	constexpr rttr::string_view C_COMPONENT_SERIALIZE_FUNC_NAME	= "serialize";
	constexpr rttr::string_view C_COMPONENT_SET_FUNC_NAME		= "set";
	constexpr rttr::string_view C_COMPONENT_SHOW_UI_FUNC_NAME	= "show_ui";

	namespace detail
	{
		//- utility function to serialize component of an entity to json
		//------------------------------------------------------------------------------------------------------------------------
		template<class TComponent>
		inline static void serialize_component(const flecs::entity& e, nlohmann::json& json)
		{
			if (const auto* c = e.get<TComponent>(); c)
			{
				const auto type_name = rttr::type::get<TComponent>().get_name().data();
				json = nlohmann::json::object();
				json[core::io::C_OBJECT_TYPE_NAME] = type_name;
				json[type_name] = core::io::to_json_object(*c);
			}
		}

		//- utility functin to add a component from a variant to an entity
		//------------------------------------------------------------------------------------------------------------------------
		template<class TComponent>
		inline static void set_component(flecs::entity& e, const rttr::variant& var)
		{
			e.set<TComponent>(std::move(var.get_value<TComponent>()));
		}

	} //- detail

} //- ecs

//- use this macro for defining a component
#define DECLARE_COMPONENT(c) \
static stringview_t name() { static constexpr stringview_t C_NAME = STRING(c); return C_NAME; } \
static void serialize(flecs::entity e, nlohmann::json& json) \
{ \
	ecs::detail::serialize_component<c>(e, json); \
} \
static void set(flecs::entity e, const rttr::variant& var) \
{ \
	ecs::detail::set_component<c>(e, var); \
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

		static void show_ui(flecs::entity e);

		std::string m_name;
		flecs::entity m_self;
		core::cuuid m_uuid;
		
		//- internal usage
		entity_proxy_t m_aabb_proxy		= 0;
		unsigned m_aabb_proxy_query_key = 0;

		RTTR_ENABLE(icomponent);
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct stransform final : public icomponent
	{
		DECLARE_COMPONENT(stransform);

		static void show_ui(flecs::entity e);

		float m_x = 0.0f, m_y = 0.0f, m_w = 0.0f, m_h = 0.0f, m_rotation = 0.0f;

		RTTR_ENABLE(icomponent);
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct ssprite final : public icomponent
	{
		DECLARE_COMPONENT(ssprite);

		static void show_ui(flecs::entity e);

		vector_t<core::spair<texture_t, material_t>> m_materials;

		core::scolor m_tint				= core::scolor(core::common_color_neutral1000);
		//- represents a rectangle in actual texture pixel size
		core::srect m_source_rectangle	= {0.0f, 0.0f, 0.0f, 0.0f};
		float m_originx					= 0.0f;
		float m_originy					= 0.0f;
		bool m_visible					= true;
		bool m_flipx					= false;
		bool m_flipy					= false;
		unsigned char m_layer			= 0;

		RTTR_ENABLE(icomponent);
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sanimation final : public icomponent
	{
		DECLARE_COMPONENT(sanimation);

		static void show_ui(flecs::entity e);

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

	//- Parent and children are represented as uuids because those are easy serializable,
	//- a flecs::entity however, is not.
	//------------------------------------------------------------------------------------------------------------------------
	struct shierarchy final : public icomponent
	{
		DECLARE_COMPONENT(shierarchy);

		static void show_ui(flecs::entity e);

		core::cuuid m_parent;
		vector_t<core::cuuid> m_children;

		RTTR_ENABLE(icomponent);
	};

} //- ecs

namespace ecs
{
	//- TODO: we do not want to have to define serialize and set etc methods for each component,
	//- this should be done automatically, for which we require a 'RTTR Object' concept and
	//- specializations for components, visualizers etc.
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(icomponent)
	{
		rttr::registration::class_<icomponent>("icomponent")
			.method("name", &icomponent::name)
			;
	};

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(sidentifier)
	{
		rttr::registration::class_<sidentifier>("sidentifier")
			.property("m_uuid", &sidentifier::m_uuid)
			.property("m_name", &sidentifier::m_name)
			.method("name", &sidentifier::name)
			.method(C_COMPONENT_SERIALIZE_FUNC_NAME, &sidentifier::serialize)
			.method(C_COMPONENT_SET_FUNC_NAME, &sidentifier::set)
			.method(C_COMPONENT_SHOW_UI_FUNC_NAME, &sidentifier::show_ui)
			;

		rttr::default_constructor<sidentifier>();
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
			.method(C_COMPONENT_SERIALIZE_FUNC_NAME, &stransform::serialize)
			.method(C_COMPONENT_SET_FUNC_NAME, &stransform::set)
			.method(C_COMPONENT_SHOW_UI_FUNC_NAME, &stransform::show_ui)
			;

		rttr::default_constructor<stransform>();
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
			.property("m_source_rectangle", &ssprite::m_source_rectangle)
			.property("m_tint", &ssprite::m_tint)
			.method(C_COMPONENT_SERIALIZE_FUNC_NAME, &ssprite::serialize)
			.method(C_COMPONENT_SET_FUNC_NAME, &ssprite::set)
			.method(C_COMPONENT_SHOW_UI_FUNC_NAME, &ssprite::show_ui)
			;

		rttr::default_constructor<ssprite>();
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
			.method(C_COMPONENT_SERIALIZE_FUNC_NAME, &sanimation::serialize)
			.method(C_COMPONENT_SET_FUNC_NAME, &sanimation::set)
			.method(C_COMPONENT_SHOW_UI_FUNC_NAME, &sanimation::show_ui)
			;

		rttr::default_constructor<sanimation>();
	};

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(shierarchy)
	{
		rttr::registration::class_<shierarchy>("shierarchy")
			.property("m_parent", &shierarchy::m_parent)
			.property("m_children", &shierarchy::m_children)
			.method(C_COMPONENT_SERIALIZE_FUNC_NAME, &shierarchy::serialize)
			.method(C_COMPONENT_SET_FUNC_NAME, &shierarchy::set)
			.method(C_COMPONENT_SHOW_UI_FUNC_NAME, &shierarchy::show_ui)
			;

		rttr::default_constructor<shierarchy>();
	};

} //- ecs