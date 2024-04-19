#pragma once
#include <core.h>

namespace raylib
{
#ifdef far
#undef far
#endif
#ifdef near
#undef near
#endif

extern "C"
{
#if defined (CORE_PLATFORM_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#endif
#define EXTERNAL_CONFIG_FLAGS
#include "raylib/raylib.h"
#include "raylib/rlgl.h"
}
} //- raylib
