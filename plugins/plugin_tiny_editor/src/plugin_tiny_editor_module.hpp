#pragma once
#include <engine.h>
#include <plugin_logging.h>
//- TODO: our editor should not be including anything from the kingdoms project
#include <plugin_kingdoms_core.h>
#include <plugin_race_human.h>
#include <plugin_kingdoms_and_empires.h>
#include <plugin_camera_system.h>
#include <lua.h>
#include <luabridge.h>

#include "editor/editor_dock_system.hpp"

namespace editor
{
	//- Editor layer. Adding this to engine layers basically enables editor functionality.
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

} //- editor