#pragma once
#include <engine.h>
#include <plugin_logging.h>
#include <plugin_kingdoms_core.h>
#include <plugin_camera_system.h>
#include <plugin_race_human.h>
#include <plugin_kingdoms_and_empires.h>
#include <lua.h>
#include <luabridge.h>

#include "editor/editor_dock_system.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	class ceditor
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

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(ceditor)
	{
		rttr::registration::class_<ceditor>("ceditor")
			.method("on_update",		&ceditor::on_update)
			.method("on_world_render",	&ceditor::on_world_render)
			.method("on_ui_render",		&ceditor::on_ui_render)
			.method("on_post_update",	&ceditor::on_post_update)
			.method("init",				&ceditor::init)
			.method("shutdown",			&ceditor::shutdown)
			;

		rttr::default_constructor<ceditor>();
	};

} //- editor