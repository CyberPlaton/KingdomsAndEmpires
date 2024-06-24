#pragma once
#include <core.h>

namespace engine
{
	//- This is a dummy layer to show how one should be defined. While creating one you dont have to inherit from it.
	//- Note: if you dont reflect a function then it will not be registered and executed,
	//- thus you can limit your layer to update only if you only define that function.
	//-
	//- When creating a new layer, inherit from this one use the declaration macro.
	//------------------------------------------------------------------------------------------------------------------------
	struct slayer
	{
		static constexpr std::string_view C_LAYER_UPDATE_FUNC_NAME		= "on_update";
		static constexpr std::string_view C_LAYER_WORLD_RENDER_FUNC_NAME= "on_world_render";
		static constexpr std::string_view C_LAYER_UI_RENDER_FUNC_NAME	= "on_ui_render";
		static constexpr std::string_view C_LAYER_POST_UPDATE_FUNC_NAME = "on_post_update";
		static constexpr std::string_view C_LAYER_INIT_FUNC_NAME		= "init";
		static constexpr std::string_view C_LAYER_SHUTDOWN_FUNC_NAME	= "shutdown";

		static bool init()					{return false;}
		static void shutdown()				{}
		static void on_update(float)		{}
		static void on_world_render()		{}
		static void on_ui_render()			{}
		static void on_post_update(float)	{}

		virtual ~slayer() = default;

		RTTR_ENABLE();
	};

} //- engine

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(slayer)
	{
		rttr::cregistrator<slayer>("slayer")
			.meth("on_update",		&slayer::on_update)
			.meth("on_world_render",&slayer::on_world_render)
			.meth("on_ui_render",	&slayer::on_ui_render)
			.meth("on_post_update", &slayer::on_post_update)
			.meth("init",			&slayer::init)
			.meth("shutdown",		&slayer::shutdown)
			;
	}

} //- engine

//- Use macro to reflect your layer, the layer functions must be declared and implemented
//------------------------------------------------------------------------------------------------------------------------
#define REGISTER_LAYER(layer) \
REFLECT_INLINE(layer) \
{ \
	rttr::cregistrator<layer>(STRINGIFY(layer)) \
		.meth("on_update",		&layer::on_update) \
		.meth("on_world_render",&layer::on_world_render) \
		.meth("on_ui_render",	&layer::on_ui_render) \
		.meth("on_post_update", &layer::on_post_update) \
		.meth("init",			&layer::init) \
		.meth("shutdown",		&layer::shutdown) \
		; \
}