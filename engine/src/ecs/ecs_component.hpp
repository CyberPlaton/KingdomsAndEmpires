#pragma once
#include <core.h>
#include <flecs.h>

namespace ecs
{
	namespace meta
	{
		constexpr std::string_view C_COMPONENT_NOT_EDITABLE		= "NOT_EDITABLE";
		constexpr std::string_view C_COMPONENT_NOT_VISIBLE		= "NOT_VISIBLE";

	} //- meta

	namespace detail
	{
		constexpr rttr::string_view C_COMPONENT_NAME_FUNC_NAME		= "name";
		constexpr rttr::string_view C_COMPONENT_SERIALIZE_FUNC_NAME = "serialize";
		constexpr rttr::string_view C_COMPONENT_SET_FUNC_NAME		= "set";
		constexpr rttr::string_view C_COMPONENT_SHOW_UI_FUNC_NAME	= "show_ui";

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

//- Macro for defining a component
//- TODO: 'automate' component registration to RTTR or at least make it more seamless.
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

//- Shortcut macro for defining a 'tag' component. A tag component does not have any data
//- and is useful in ecs queries and systems.
//- Declaring a tag does not require registration to RTTR, it is done automatically.
#define DECLARE_TAG(c) \
struct c final : public ecs::icomponent \
{ \
	DECLARE_COMPONENT(c); \
	RTTR_ENABLE(ecs::icomponent) \
}; \
REFLECT_INLINE(c) \
{ \
	rttr::registration::class_<c>(STRING(c)) \
		.method("name", &c::name) \
		; \
	rttr::default_constructor<c>(); \
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

	//- 'Transform' for camera is included in the camera component.
	//------------------------------------------------------------------------------------------------------------------------
	struct scamera final : public icomponent
	{
		DECLARE_COMPONENT(scamera);

		vec2_t m_position			= { 0.0f, 0.0f };
		vec2_t m_offset				= { 0.0f, 0.0f };
		float m_zoom				= 0.0f;
		float m_rotation			= 0.0f;
		core::srect m_viewrect		= { 0.0f, 0.0f, 0.0f, 0.0f };
		bool m_active				= false;
		renderlayer_t m_layer		= 0;

		RTTR_ENABLE(icomponent);
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sidentifier final : public icomponent
	{
		DECLARE_COMPONENT(sidentifier);

		static void show_ui(flecs::entity e);

		string_t m_name;
		core::cuuid m_uuid;

		RTTR_ENABLE(icomponent);
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct stransform final : public icomponent
	{
		DECLARE_COMPONENT(stransform);

		static void show_ui(flecs::entity e);

		//- local space position. To get the final position use a transform function
		vec2_t m_position;

		//- scale of the final sprite, where 1 is actual size
		vec2_t m_scale;

		//- rotation in radians
		float m_angles;

		RTTR_ENABLE(icomponent);
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct ssprite final : public icomponent
	{
		DECLARE_COMPONENT(ssprite);

		enum flags : uint8_t
		{
			flags_none		= BIT(0),
			flags_visible	= BIT(1),
			flags_flipx		= BIT(2),
			flags_flipy		= BIT(3),
		};

		static void show_ui(flecs::entity e);

		vector_t<core::spair<texture_t, material_t>> m_materials;

		core::srect m_source_rectangle	= { 0.0f, 0.0f, 0.0f, 0.0f }; //- represents a rectangle in actual texture pixel size
		core::scolor m_tint				= core::scolor(core::common_color_neutral1000);
		int m_flags						= flags_visible;
		unsigned char m_layer			= 0;

		RTTR_ENABLE(icomponent);
	};

	//- TODO: Animations are to be split into 'Frame' based animations as defined here and into 'Skeletal' animations,
	//- both will have their own systems going with as well as components.
	//- Thus this component will be moved into plugin_frame_animation.
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

	namespace tag
	{
		//- Tag an entity as invisible, i.e. it will not be drawn if it has a sprite component.
		//------------------------------------------------------------------------------------------------------------------------
		DECLARE_TAG(sinvisible);

	} //- tag

} //- ecs

namespace rttr
{
	//- Utility class for RTTR component registration. Does register default serialize and deserialize functions,
	//- but not show_ui(), that is defined individually where required.
	//------------------------------------------------------------------------------------------------------------------------
	template<class TComponent>
	class ccomponent : public cregistrator<TComponent>
	{
	public:
		ccomponent(rttr::string_view name) :
			cregistrator<TComponent>(name)
		{
			register_common_component_functions();
		}

	private:
		void register_common_component_functions()
		{
			this->meth(ecs::detail::C_COMPONENT_NAME_FUNC_NAME, &TComponent::name)
				.meth(ecs::detail::C_COMPONENT_SERIALIZE_FUNC_NAME, &TComponent::serialize)
				.meth(ecs::detail::C_COMPONENT_SET_FUNC_NAME, &TComponent::set)
				;
		}
	};

} //- rttr

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
		rttr::ccomponent<sidentifier>("sidentifier")
			.prop("m_uuid", &sidentifier::m_uuid)
			.prop("m_name", &sidentifier::m_name)
			.meth(detail::C_COMPONENT_SHOW_UI_FUNC_NAME, &sidentifier::show_ui)
			;
	};

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(stransform)
	{
		rttr::ccomponent<stransform>("stransform")
			.prop("m_position", &stransform::m_position)
			.prop("m_scale", &stransform::m_scale)
			.prop("m_angles", &stransform::m_angles)
			.meth(detail::C_COMPONENT_SHOW_UI_FUNC_NAME, &stransform::show_ui)
			;
	};

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(ssprite)
	{
		rttr::ccomponent<ssprite>("ssprite")
			.prop("m_materials", &ssprite::m_materials)
			.prop("m_flags", &ssprite::m_flags)
			.prop("m_layer", &ssprite::m_layer)
			.prop("m_source_rectangle", &ssprite::m_source_rectangle)
			.prop("m_tint", &ssprite::m_tint)
			.meth(detail::C_COMPONENT_SHOW_UI_FUNC_NAME, &ssprite::show_ui)
			;
	};

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(sanimation)
	{
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
	};

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(shierarchy)
	{
		rttr::ccomponent<shierarchy>("shierarchy")
			.prop("m_parent", &shierarchy::m_parent)
			.prop("m_children", &shierarchy::m_children)
			.meth(detail::C_COMPONENT_SHOW_UI_FUNC_NAME, &shierarchy::show_ui)
			;
	};

} //- ecs