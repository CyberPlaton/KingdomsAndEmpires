#pragma once
#include "kingdoms/map/kingdoms_map_generator.hpp"
#include "kingdoms/map/kingdoms_tileset_generator.hpp"
#include "kingdoms_game_context.hpp"

namespace kingdoms
{
	//- Main game application layer.
	//------------------------------------------------------------------------------------------------------------------------
	class ckingdoms_layer_ui final
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


} //- kingdoms