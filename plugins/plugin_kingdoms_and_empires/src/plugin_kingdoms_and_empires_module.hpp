#pragma once
#include <engine.h>
#include "config.hpp"
#include <plugin_kingdoms_core.h>
#include <plugin_camera_system.h>
#include <plugin_race_human.h>
#include <lua.h>
#include <luabridge.h>

//------------------------------------------------------------------------------------------------------------------------
class KINGDOMS_API cgame
{
public:
	static bool init();
	static void shutdown();
	static void on_update(float dt);
	static void on_world_render();
	static void on_ui_render();
	static void on_post_update(float dt);

	RTTR_ENABLE();
};