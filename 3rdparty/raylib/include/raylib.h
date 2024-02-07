#pragma once
namespace raylib
{
#ifdef far
#undef far
#endif
#ifdef near
#undef near
#endif
#include "raylib/raylib.h"
#include "raylib/rlgl.h"
#include "raylib/raymath.h"
} //- raylib