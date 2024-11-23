#pragma once
#include <core.h>

namespace kingdoms
{
	namespace tiled
	{
		//------------------------------------------------------------------------------------------------------------------------
		struct stiled_tileset_reference final
		{
			unsigned m_first_gid = 0;
			string_t m_source;

			RTTR_ENABLE();
		};

		//------------------------------------------------------------------------------------------------------------------------
		struct stiled_map_chunk final
		{
			vector_t<size_t> m_data;
			unsigned m_height = 0;
			unsigned m_width = 0;
			int m_x = 0; //- Origin of the top-left tile. Should probably convert from negative space to positive.
			int m_y = 0;

			RTTR_ENABLE();
		};

		//------------------------------------------------------------------------------------------------------------------------
		struct stiled_map_layer final
		{
			vector_t<stiled_map_chunk> m_chunks;
			string_t m_name;
			string_t m_type;
			int m_id = 0;
			unsigned m_height = 0;
			unsigned m_width = 0;
			int m_x = 0;
			int m_y = 0;
			int m_opacity = 0;
			int m_startx = 0;
			int m_starty = 0;
			bool m_visible = true;

			RTTR_ENABLE();
		};

		//- Map data imported from Tiled program as json with fileextension .tmj.
		//------------------------------------------------------------------------------------------------------------------------
		struct stiled_map_data final
		{
			vector_t<stiled_map_layer> m_layers;
			vector_t<stiled_tileset_reference> m_tilesets;
			string_t m_orientation;
			string_t m_render_order;
			string_t m_tiled_version;
			string_t m_type;
			string_t m_version;
			int m_compression_level = -1;
			int m_next_layer_id = 0;
			int m_next_object_id = 0;
			unsigned m_width = 0;
			unsigned m_height = 0;
			unsigned m_tile_height = 0;
			unsigned m_tile_width = 0;
			bool m_infinite = false;

			RTTR_ENABLE();
		};

	} //- tiled

} //- kingdoms