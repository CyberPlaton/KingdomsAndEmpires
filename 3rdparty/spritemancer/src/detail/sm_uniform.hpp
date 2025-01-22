#pragma once
#include "sm_config.hpp"

namespace sm
{
	bgfx::UniformHandle create_uniform(stringview_t name, uniform_type type);
	void destroy_uniform(bgfx::UniformHandle handle);

} //- sm
