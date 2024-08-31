#include "sm_renderer_bgfx.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::prepare_device()
	{
		//- no-op
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult crenderer_bgfx::init_device(void* nwh, unsigned w, unsigned h, bool fullscreen, bool vsync)
	{
		bgfx::Init config;

		config.resolution.width = w;
		config.resolution.height = h;
		config.resolution.reset = vsync? BGFX_RESET_VSYNC : BGFX_RESET_NONE;
		config.platformData.nwh = nwh;

		//- In case we want to use single-threaded rendering call this once
		//- bgfx::renderFrame();

		if (bgfx::init(config))
		{
			return opresult_ok;
		}
		return opresult_fail;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult crenderer_bgfx::shutdown_device()
	{
		bgfx::shutdown();
		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::prepare_frame()
	{
		//- This dummy draw call is here to make sure that view 0 is cleared if no other draw calls are submitted to view 0
		bgfx::touch(0);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::display_frame()
	{
		//- Advance to next frame. Process submitted rendering primitives
		bgfx::frame();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::update_viewport(const vec2_t& position, const vec2_t& size)
	{
		S_X = (uint16_t)/*position.x*/0;
		S_Y = (uint16_t)/*position.y*/0;
		S_W = (uint16_t)size.x;
		S_H = (uint16_t)size.y;

		S_CAMERA.offset = { S_W / 2.0f, S_H / 2.0f };
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::blendmode(sblending mode)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::clear(const slayer& layer, bool depth)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	bool crenderer_bgfx::begin(const slayer& layer)
	{
		if (is_valid(layer.m_target))
		{
			//- Check some flags and do adjustments
			if (algorithm::bit_check(layer.m_flags, layer_flags_2d))
			{
				//- Push orthogonal camera
			}

			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::draw(const slayer& layer)
	{
		for (const auto& command : layer.m_commands)
		{
			command.execute();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::end(const slayer& layer)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	bool crenderer_bgfx::combine(const slayer& layer)
	{
		//- Draw the target texture from layer on top of previous target
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::update_texture_gpu(uint64_t id, unsigned w, unsigned h, texture_format format, const void* data)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::update_texture_cpu(uint64_t id, unsigned w, unsigned h, texture_format format, void*& data)
	{

	}

} //- sm

RTTR_REGISTRATION
{
	using namespace sm;

	rttr::cregistrator<crenderer_bgfx>("crenderer_bgfx")
		;
}