#pragma once
#include <core.h>

namespace kingdoms
{
	namespace tiled
	{
		//- Tileset data imported from Tiled as json with fileextension .tsj.
		//------------------------------------------------------------------------------------------------------------------------
		struct stiled_tileset_data final
		{
			string_t m_name;
			string_t m_type;
			string_t m_image;
			string_t m_version;
			string_t m_tiled_version;
			unsigned m_columns = 0;
			unsigned m_image_height = 0;
			unsigned m_image_width = 0;
			unsigned m_margin = 0;
			unsigned m_spacing = 0;
			unsigned m_tile_count = 0;
			unsigned m_tile_height = 0;
			unsigned m_tile_width = 0;

			RTTR_ENABLE();
		};

		//- Image data for a spriteatlas generated using 'crunch' in json format.
		//------------------------------------------------------------------------------------------------------------------------
		struct stileset_image_data final
		{
			struct simages
			{
				string_t m_name;
				int m_x = 0;
				int m_y = 0;
				int m_width = 0;
				int m_height = 0;

				RTTR_ENABLE();
			};

			struct stextures
			{
				string_t m_name;
				vector_t<simages> m_images;

				RTTR_ENABLE();
			};

			vector_t<stextures> m_textures;

			RTTR_ENABLE();
		};

	} //- tiled

	//- Responsible for generating and loading a spriteatlas from Tiled '.tsj' json data.
	//------------------------------------------------------------------------------------------------------------------------
	struct stileset_generator final
	{
		static bool load_from_file(stringview_t tilesetname);
		static bool load_from_memory(const uint8_t* string, unsigned size);
	};

} //- kingdoms