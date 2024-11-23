#pragma once
#include <core.h>

namespace kingdoms
{
	//- Common game related data and settings that are used in many places and formalize
	//------------------------------------------------------------------------------------------------------------------------
	struct sgame final
	{
		static constexpr stringview_t C_RACE_DATA_FILEPATH	= "/race/";
		static constexpr stringview_t C_MAP_DATA_FILEPATH	= "/maps/";
		static constexpr stringview_t C_MAP_DATA_EXT		= ".tmj";
		static constexpr stringview_t C_TILESET_DATA_EXT	= ".tsj";
		static constexpr stringview_t C_ATLAS_DATA_EXT		= ".tsj";
	};

} //- kingdoms