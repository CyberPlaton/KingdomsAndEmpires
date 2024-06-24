#pragma once
#include <engine.h>
#include <plugin_logging.h>
#include <lua.h>
#include <luabridge.h>
#include <spritemancer.h>

//------------------------------------------------------------------------------------------------------------------------
class cexample_app_layer final : engine::slayer
{
public:
	bool init();
	void shutdown();
	void on_update(float dt);
	void on_world_render();
	void on_ui_render();
	void on_post_update(float dt);
};

REGISTER_LAYER(cexample_app_layer);