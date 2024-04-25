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

	//------------------------------------------------------------------------------------------------------------------------
	struct sexample_singleton : isingleton
	{
		DECLARE_SINGLETON(sexample_singleton);

		umap_t<std::string, bool> m_data;

		RTTR_ENABLE(isingleton);
	};

} //- ecs

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(sexample_singleton)
	{
		rttr::registration::class_<sexample_singleton>("sexample_singleton")
			.property("m_data", &sexample_singleton::m_data)
			.method("serialize", &sexample_singleton::serialize)
			.method("set", &sexample_singleton::set)
			;

		rttr::default_constructor<sexample_singleton>();
		rttr::default_constructor<umap_t<std::string, bool>>();
	};

} //- ecs