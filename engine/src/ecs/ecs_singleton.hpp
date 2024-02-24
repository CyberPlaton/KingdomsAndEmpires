#pragma once
#include <core.h>
#include <flecs.h>

namespace ecs
{
	constexpr std::string_view C_SINGLETON_SERIALIZE_FUNC_NAME = "serialize";
	constexpr std::string_view C_SINGLETON_SET_FUNC_NAME = "set";

	namespace detail
	{
		//- utility function to serialize a singleton component of a world to json
		//------------------------------------------------------------------------------------------------------------------------
		template<class TComponent>
		inline static void serialize_singleton(const flecs::world& w, nlohmann::json& json)
		{
			if (const auto* c = w.get<TComponent>(); c)
			{
				const auto type_name = rttr::type::get<TComponent>().get_name().data();
				json = nlohmann::json::object();
				json[core::io::C_OBJECT_TYPE_NAME] = type_name;
				json[type_name] = core::io::to_json_object(*c);
			}
		}

		//- utility function to add a singleton component from a variant to a world
		//------------------------------------------------------------------------------------------------------------------------
		template<class TComponent>
		inline static void set_singleton(flecs::world& w, const rttr::variant& var)
		{
			w.set<TComponent>(std::move(var.get_value<TComponent>()));
		}

	} //- detail

} //- ecs

//- use this macro for defining a singleton component
#define DECLARE_SINGLETON(c) \
static stringview_t name() { static constexpr stringview_t C_NAME = STRING(c); return C_NAME; } \
static void serialize(const flecs::world& w, nlohmann::json& json) \
{ \
	ecs::detail::serialize_singleton<c>(w, json); \
} \
static void set(flecs::world& w, const rttr::variant& var) \
{\
	ecs::detail::set_singleton<c>(w, var); \
}

namespace ecs
{
	//- base class for all singletons
	//------------------------------------------------------------------------------------------------------------------------
	struct isingleton
	{
		static stringview_t name() { static constexpr stringview_t C_NAME = "isingleton"; return C_NAME; };

		RTTR_ENABLE();
	};

	//- singleton contains all defined phases for systems to run on.
	//- Does not need to be serializable, as its initialized on each world start.
	//- Note: to register a system that should run on update tick do
	//- .kind<ecs::ssystem_phases::son_update>()
	//- in flecs system builder (system definition/creation).
	//------------------------------------------------------------------------------------------------------------------------
	struct ssystem_phases : isingleton
	{
		DECLARE_SINGLETON(ssystem_phases);

		struct son_update {};
		struct son_world_render {};
		struct son_ui_render {};

		inline static const rttr::type C_ON_UPDATE_PHASE = rttr::type::get<son_update>();
		inline static const rttr::type C_ON_WORLD_RENDER_PHASE = rttr::type::get<son_world_render>();
		inline static const rttr::type C_ON_UI_RENDER_PHASE = rttr::type::get<son_ui_render>();

		RTTR_ENABLE(isingleton);
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct stools_and_visualizers : isingleton
	{
		DECLARE_SINGLETON(stools_and_visualizers);

		umap_t<std::string, bool> m_activated_table;

		RTTR_ENABLE(isingleton);
	};

} //- ecs

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(stools_and_visualizers)
	{
		rttr::registration::class_<stools_and_visualizers>("stools_and_visualizers")
			.property("m_activated_table", &stools_and_visualizers::m_activated_table)
			.method("serialize", &stools_and_visualizers::serialize)
			.method("set", &stools_and_visualizers::set)
			;

		rttr::default_constructor<stools_and_visualizers>();
		rttr::default_constructor<umap_t<std::string, bool>>();
	};

} //- ecs