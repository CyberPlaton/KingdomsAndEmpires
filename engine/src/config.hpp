#pragma once
#include <core.h>

namespace engine::cfg
{
	//- 
	//------------------------------------------------------------------------------------------------------------------------
	constexpr auto C_MODULE_RESERVE_COUNT = 256;

	//- Maximum count of queries to be processed at a time. When submitting more than that queries are dropped
	//------------------------------------------------------------------------------------------------------------------------
	constexpr auto C_ECS_QUERY_COUNT_MAX = 256;

} //- engine::cfg