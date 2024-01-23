#pragma once
#include <plugin_module_example.h>

namespace module_example
{
	namespace component
	{
		struct stargeting
		{
			static inline constexpr auto C_TARGET_COOLDOWN_INVALID = -100.0f;
			static inline constexpr auto C_TARGET_COOLDOWN_TIMER = 3.0f;

			core::cuuid m_next_target = core::cuuid::C_INVALID_UUID;
			float m_cooldown = C_TARGET_COOLDOWN_INVALID;
		};

	} //- component

} //- module_example