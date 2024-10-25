#pragma once
#include "plugin_module_example_common.hpp"
#include "components/example_components.hpp"
#include "systems/example_system.hpp"

namespace module_example
{
	//------------------------------------------------------------------------------------------------------------------------
	struct EXAMPLE_API smy_module final
	{
		smy_module() = default;
		smy_module(ecs::cworld* w);

		RTTR_ENABLE();
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct EXAMPLE_API smy_second_module final
	{
		smy_second_module() = default;
		smy_second_module(ecs::cworld* w);

		RTTR_ENABLE();
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct EXAMPLE_API smy_third_module final
	{
		smy_third_module() = default;
		smy_third_module(ecs::cworld* w);

		RTTR_ENABLE();
	};

} //- module_example