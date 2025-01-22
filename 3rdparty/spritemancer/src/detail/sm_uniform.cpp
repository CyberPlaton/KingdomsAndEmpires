#include "sm_uniform.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	bgfx::UniformHandle create_uniform(stringview_t name, uniform_type type)
	{
		return bgfx::createUniform(name.data(), detail::to_bgfx_uniform_type(type), 1);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void destroy_uniform(bgfx::UniformHandle handle)
	{
		if (bgfx::isValid(handle))
		{
			bgfx::destroy(handle);
		}
	}

} //- sm
