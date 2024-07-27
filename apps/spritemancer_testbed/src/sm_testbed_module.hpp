#pragma once
#include <engine.h>
#include <plugin_ai.h>
#include <lua.h>
#include <luabridge.h>
#include <spritemancer.h>
#include <unittest.h>

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