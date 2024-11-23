#pragma once
#include "../../kingdoms_common.hpp"
#include "tiled_integration/kingdoms_tiled_map.hpp"
#include "tiled_integration/kingdoms_tiled_tileset.hpp"

namespace kingdoms
{
	namespace detail
	{
		//------------------------------------------------------------------------------------------------------------------------
		struct smap_generation_context final
		{
			//- General information and storage
			string_t m_name;
			string_t m_relative_path;
			string_t m_path;
			tiled::stiled_map_data m_map_data;

			//- Dynamic data changed frequently on generation
			unsigned m_processed_tileset_index = 0; //- Refers to the index of the tileset currently being generated
		};

	} //- detail

	using map_generation_context_ref_t = ref_t<detail::smap_generation_context>;

} //- kingdoms