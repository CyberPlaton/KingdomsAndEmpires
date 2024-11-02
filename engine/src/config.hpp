#pragma once
#include <core.h>
#include "version.hpp"

namespace engine::cfg
{
	//- 
	//------------------------------------------------------------------------------------------------------------------------
	constexpr auto C_MODULE_RESERVE_COUNT = 256;

	//- Maximum count of queries to be processed at a time. When submitting more than that queries are dropped
	//------------------------------------------------------------------------------------------------------------------------
	constexpr auto C_ECS_QUERY_COUNT_MAX = 256;

	//- Whether the ecs world module manager should gather and retain module and system dependency information, such as
	//- module-to-module, system-to-module and system-to-system dependencies etc., all currently loaded systems and modules etc.
	//------------------------------------------------------------------------------------------------------------------------
	constexpr auto C_ECS_MODULES_AND_SYSTEMS_GATHER_INFORMATION = true;

} //- engine::cfg