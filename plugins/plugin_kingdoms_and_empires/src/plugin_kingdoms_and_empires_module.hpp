#pragma once
#include <engine.h>
#include <plugin_logging.h>
#include <plugin_kingdoms_core.h>
#include <plugin_camera_system.h>
#include <plugin_race_human.h>
#include <lua.h>
#include <luabridge.h>

//------------------------------------------------------------------------------------------------------------------------
class cgame
{
public:
	static void on_update(float dt);
	static void on_world_render();
	static void on_ui_render();
	static void on_post_update(float dt);

	RTTR_ENABLE();
};