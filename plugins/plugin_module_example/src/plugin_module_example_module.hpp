#pragma once
#include "plugin_module_example_common.hpp"
#include "components/example_components.hpp"
#include "systems/example_system.hpp"

namespace module_example
{
	//------------------------------------------------------------------------------------------------------------------------
	struct EXAMPLE_API smy_module final
	{
		DECLARE_MODULE(smy_module);

		static ecs::modules::sconfig config();

		RTTR_ENABLE();
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct EXAMPLE_API smy_second_module final
	{
		DECLARE_MODULE(smy_second_module);

		static ecs::modules::sconfig config();

		RTTR_ENABLE();
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct EXAMPLE_API smy_third_module final
	{
		DECLARE_MODULE(smy_third_module);

		static ecs::modules::sconfig config();

		RTTR_ENABLE();
	};

} //- module_example