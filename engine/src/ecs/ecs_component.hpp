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
#define DECLARE_TAG(c, ...) \
struct __VA_ARGS__ c final : public ecs::icomponent \
{ \
	DECLARE_COMPONENT(c); \
	RTTR_ENABLE(ecs::icomponent) \
};

namespace ecs
{
	//- Base class for all components
	//- TODO: we do not want to have to define serialize and set etc methods for each component,
	//- this should be done automatically, for which we require a 'RTTR Object' concept and
	//- specializations for components, visualizers etc.
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

		static void show_ui(flecs::entity e);

		vec2_t m_position			= { 0.0f, 0.0f };
		vec2_t m_offset				= { 0.0f, 0.0f };
		float m_zoom				= 0.0f;
		float m_rotation			= 0.0f;
		core::srect m_viewrect		= { 0.0f, 0.0f, 0.0f, 0.0f };
		bool m_active				= false;
		unsigned m_renderlayer		= 0;

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

		vec2_t m_position	= { 0.0f, 0.0f };	//- local space position. To get the final position use a transform function
		vec2_t m_size		= { 0.0f, 0.0f };	//- size of object in pixels
		float m_angles		= 0.0f;				//- rotation in radians

		RTTR_ENABLE(icomponent);
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct ssprite final : public icomponent
	{
		DECLARE_COMPONENT(ssprite);

		static void show_ui(flecs::entity e);

		core::srect m_source_rectangle	= { 0.0f, 0.0f, 0.0f, 0.0f }; //- represents a rectangle in actual texture pixel size
		core::scolor m_tint				= core::scolor(core::common_color_neutral1000);
		int m_flags						= 0;
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
	class ccomponent final : public cregistrator<TComponent>
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