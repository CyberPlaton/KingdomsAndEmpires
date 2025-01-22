#pragma once
#include "sm_config.hpp"

namespace sm
{
	bgfx::FrameBufferHandle create_rendertarget(unsigned w, unsigned h);
	bgfx::FrameBufferHandle create_rendertarget(framebuffer_ratio ratio);
	bgfx::TextureHandle rendertarget_texture(bgfx::FrameBufferHandle handle);
	void destroy_rendertarget(bgfx::FrameBufferHandle handle);

} //- sm
