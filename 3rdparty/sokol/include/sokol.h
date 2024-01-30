#pragma once
#define SOKOL_DLL
#if defined (_WIN32) || defined (_WIN64)
#define SOKOL_D3D11
#elif defined (__linux__)
#define SOKOL_GLCORE33
#elif defined (__APPLE__)
#define SOKOL_METAL
#else
#endif
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_gp.h"
#include "sokol/sokol_app.h"
#include "sokol/sokol_glue.h"