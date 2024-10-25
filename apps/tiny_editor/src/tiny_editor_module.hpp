#pragma once
#include <engine.h>
#include <plugin_module_example.h>
#include <plugin_camera_system.h>
#include <lua.h>
#include <luabridge.h>
#include <tiny_editor.h>
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