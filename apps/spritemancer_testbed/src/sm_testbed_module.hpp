#pragma once
#include <engine.h>
#include <plugin_logging.h>
#include <lua.h>
#include <luabridge.h>
#include <spritemancer.h>

//------------------------------------------------------------------------------------------------------------------------
class cexample_app_layer final
{
public:
	static bool init();
	static void shutdown();
	static void on_update(float dt);
	static void on_world_render();
	static void on_ui_render();
	static void on_post_update(float dt);
};

REGISTER_LAYER(cexample_app_layer);

//------------------------------------------------------------------------------------------------------------------------
class cexample_bad_app_layer final
{
public:
	bool init()						{ return true;}
	static void shutdown()			{}
	static void on_update(float dt) {}
	static void on_world_render()	{}
	static void on_ui_render()		{}
};

//------------------------------------------------------------------------------------------------------------------------
REFLECT_INLINE(cexample_bad_app_layer)
{
	rttr::cregistrator<cexample_bad_app_layer>("cexample_bad_app_layer")
		.meth("on_update",		&cexample_bad_app_layer::on_update)
		.meth("on_world_render",&cexample_bad_app_layer::on_world_render)
		.meth("on_ui_render",	&cexample_bad_app_layer::on_ui_render)
		.meth("init",			&cexample_bad_app_layer::init)
		.meth("shutdown",		&cexample_bad_app_layer::shutdown)
		;
}