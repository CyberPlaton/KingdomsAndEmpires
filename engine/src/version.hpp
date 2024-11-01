#pragma once
#include <core.h>

#define ENGINE_VERSION_MAJOR 0
#define ENGINE_VERSION_MINOR 0
#define ENGINE_VERSION_PATCH 0

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	struct sinfo final
	{
		static inline const auto C_VERSION_MAJOR = ENGINE_VERSION_MAJOR;
		static inline const auto C_VERSION_MINOR = ENGINE_VERSION_MINOR;
		static inline const auto C_VERSION_PATCH = ENGINE_VERSION_PATCH;
		static inline const auto C_VERSION = fmt::format("{}.{}.{}", C_VERSION_MAJOR, C_VERSION_MINOR, C_VERSION_PATCH);
	};

} //- engine