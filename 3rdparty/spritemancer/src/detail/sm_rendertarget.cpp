#include "sm_rendertarget.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	bgfx::FrameBufferHandle create_rendertarget(unsigned w, unsigned h)
	{
		return bgfx::createFrameBuffer((uint16_t)w, (uint16_t)h, texture_format::RGBA8, BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bgfx::FrameBufferHandle create_rendertarget(framebuffer_ratio ratio)
	{
		return bgfx::createFrameBuffer(detail::to_bgfx_ratio(ratio), texture_format::RGBA8, BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bgfx::TextureHandle rendertarget_texture(bgfx::FrameBufferHandle handle)
	{
		return bgfx::getTexture(handle);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void destroy_rendertarget(bgfx::FrameBufferHandle handle)
	{
		if (bgfx::isValid(handle))
		{
			bgfx::destroy(handle);
		}
	}

} //- sm
