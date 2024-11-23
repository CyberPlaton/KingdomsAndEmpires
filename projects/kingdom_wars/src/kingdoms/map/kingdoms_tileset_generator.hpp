#pragma once
#include "kingdoms_map_generation_context.hpp"

namespace kingdoms
{
	//- Responsible for generating and loading a spriteatlas from Tiled '.tsj' json data.
	//------------------------------------------------------------------------------------------------------------------------
	struct stileset_generator final
	{
		static bool load_from_file(map_generation_context_ref_t& ctx, stringview_t tilesetname);
		static bool load_from_memory(map_generation_context_ref_t& ctx, const uint8_t* string, unsigned size);
	};

} //- kingdoms