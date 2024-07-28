#pragma once
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
#define EXTERNAL_CONFIG_FLAGS
#include "../src/raylib/raylib.h"
#include "../src/raylib/rlgl.h"
}
} //- raylib