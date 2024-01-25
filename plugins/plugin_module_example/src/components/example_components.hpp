#pragma once
#include "../plugin_module_example_common.hpp"

namespace module_example
{
	//------------------------------------------------------------------------------------------------------------------------
	struct EXAMPLE_API stargeting_component
	{
		static inline constexpr auto C_TARGET_COOLDOWN_INVALID = -100.0f;
		static inline constexpr auto C_TARGET_COOLDOWN_TIMER = 3.0f;

		core::cuuid m_next_target = core::cuuid::C_INVALID_UUID;
		float m_cooldown = C_TARGET_COOLDOWN_INVALID;
	};

} //- module_example