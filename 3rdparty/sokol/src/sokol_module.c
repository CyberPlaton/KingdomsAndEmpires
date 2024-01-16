#include "sokol_module.h"

#define SOKOL_IMPL
#if defined (_WIN32) ||defined(_WIN64)
#define SOKOL_D3D11
#elif defined(__linux__)
#define SOKOL_GLCORE33
#elif defined(__APPLE__)
#define SOKOL_METAL
#endif
#ifdef sokol_EXPORTS
#define SOKOL_DLL
#endif
#include <sokol/sokol_gfx.h>
#include <sokol/sokol_gp.h>
#include <sokol/sokol_log.h>