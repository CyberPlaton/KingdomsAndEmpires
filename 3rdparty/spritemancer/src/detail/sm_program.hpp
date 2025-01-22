#pragma once
#include "sm_config.hpp"

namespace sm
{
	bgfx::ProgramHandle create_program(bgfx::ShaderHandle vs, bgfx::ShaderHandle ps);
	bgfx::ProgramHandle create_program(bgfx::ShaderHandle cs);
	void destroy_program(bgfx::ProgramHandle handle);

} //- sm