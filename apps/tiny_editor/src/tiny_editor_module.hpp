#pragma once
#include <engine.h>
#include <plugin_module_example.h>
//- TODO: we should not be including any of these in order to work with the editor,
//- however we do this for testing as of now; remove later
#include <plugin_kingdoms_core.h>
#include <plugin_kingdoms_and_empires.h>
#include <plugin_race_human.h>
#include <plugin_status_effects.h>
//-
#include <plugin_camera_system.h>
#include <plugin_render_system.h>
/*#include <plugin_kingdoms_and_empires.h>*/
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