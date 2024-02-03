#pragma once
#include <core.h>
#define STBI_NO_THREAD_LOCALS
#define STBI_MALLOC		CORE_MALLOC
#define STBI_FREE		CORE_FREE
#define STBI_REALLOC	CORE_REALLOC
#include "stb/stb_image.h"