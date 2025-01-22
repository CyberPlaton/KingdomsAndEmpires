#include "sm_program.hpp"
#include "sm_shader.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	bgfx::ProgramHandle create_program(bgfx::ShaderHandle vs, bgfx::ShaderHandle ps)
	{
		return bgfx::createProgram(vs, ps);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bgfx::ProgramHandle create_program(bgfx::ShaderHandle cs)
	{
		return bgfx::createProgram(cs);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void destroy_program(bgfx::ProgramHandle handle)
	{
		if (bgfx::isValid(handle))
		{
			bgfx::destroy(handle);
		}
	}

} //- sm
