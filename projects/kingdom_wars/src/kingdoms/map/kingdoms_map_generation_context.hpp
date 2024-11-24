#pragma once
#include "../../kingdoms_common.hpp"
#include "tiled_integration/kingdoms_tiled_map.hpp"
#include "tiled_integration/kingdoms_tiled_tileset.hpp"
#include <spritemancer.h>

namespace kingdoms
{
	namespace detail
	{
		//------------------------------------------------------------------------------------------------------------------------
		struct smap_generation_context final
		{
			struct slayer_information final
			{
				string_t m_name;
				float m_tile_width = 0.0f;
				float m_tile_height = 0.0f;
				unsigned m_tile_count_x = 0;
				unsigned m_tile_count_y = 0;
				sm::spriteatlas_handle_t m_atlas_handle = 0;
				sm::texture_handle_t m_atlas_texture_handle = 0;
			};

			//- Mapping: { map name, { layer name, layer info }}
			using map_layer_info_t = umap_t<string_t, umap_t<string_t, slayer_information>>;
			//- Mapping: { map name, { gid, layer name }}
			using map_layer_info_mapping_t = umap_t<string_t, umap_t<int, string_t>>;
			//- Mapping: { map name, { gid, texture name }}
			using map_gid_texture_mapping_t = umap_t<string_t, umap_t<int, string_t>>;

			//- General information and storage
			string_t m_name;
			string_t m_relative_path;
			string_t m_path;
			tiled::stiled_map_data m_map_data;

			map_layer_info_t m_layer_info_data;
			map_layer_info_mapping_t m_map_layer_mapping_data;
			map_gid_texture_mapping_t m_map_mapping_data;

			//- Dynamic data changed frequently on generation
			unsigned m_processed_tileset_index = 0; //- Refers to the index of the tileset currently being generated
		};

	} //- detail

	using map_generation_context_ref_t = ref_t<detail::smap_generation_context>;

} //- kingdoms