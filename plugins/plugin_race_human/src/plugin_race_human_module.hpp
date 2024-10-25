#pragma once
#include <plugin_kingdoms_core.h>
#include "config.hpp"

namespace races
{
	//------------------------------------------------------------------------------------------------------------------------
	struct HUMAN_API shuman : kingdoms::irace
	{
		DECLARE_COMPONENT(shuman);

		RTTR_ENABLE(kingdoms::irace);
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct HUMAN_API shuman_race_module final
	{
		shuman_race_module() = default;
		shuman_race_module(ecs::cworld* w);

		RTTR_ENABLE();
	};

} //- races