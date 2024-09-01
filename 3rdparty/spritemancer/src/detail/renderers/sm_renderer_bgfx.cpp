#include "sm_renderer_bgfx.hpp"
#include "../sm_vertices.hpp"

namespace sm
{
	namespace
	{
		static unsigned S_X = 0, S_Y = 0, S_W = 0, S_H = 0;

		//------------------------------------------------------------------------------------------------------------------------
		void screen_space_quad(bool _originBottomLeft, float _width = 1.0f, float _height = 1.0f)
		{
			if (3 == bgfx::getAvailTransientVertexBuffer(3, vertex_layouts::spostexcoord::S_LAYOUT))
			{
				bgfx::TransientVertexBuffer vb;
				bgfx::allocTransientVertexBuffer(&vb, 3, vertex_layouts::spostexcoord::S_LAYOUT);
				vertex_layouts::spostexcoord* vertex = (vertex_layouts::spostexcoord*)vb.data;

				const float minx = -_width;
				const float maxx = _width;
				const float miny = 0.0f;
				const float maxy = _height * 2.0f;

				const float minu = -1.0f;
				const float maxu = 1.0f;

				const float zz = 0.0f;

				float minv = 0.0f;
				float maxv = 2.0f;

				if (_originBottomLeft)
				{
					float temp = minv;
					minv = maxv;
					maxv = temp;

					minv -= 1.0f;
					maxv -= 1.0f;
				}

				vertex[0].m_position.x = minx;
				vertex[0].m_position.y = miny;
				vertex[0].m_position.z = zz;
				vertex[0].m_uv.x = minu;
				vertex[0].m_uv.y = minv;

				vertex[1].m_position.x = maxx;
				vertex[1].m_position.y = miny;
				vertex[1].m_position.z = zz;
				vertex[1].m_uv.x = maxu;
				vertex[1].m_uv.y = minv;

				vertex[2].m_position.x = maxx;
				vertex[2].m_position.y = maxy;
				vertex[2].m_position.z = zz;
				vertex[2].m_uv.x = maxu;
				vertex[2].m_uv.y = maxv;

				bgfx::setVertexBuffer(0, &vb);
			}
		}

	} //- unnamed

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
			S_W = w;
			S_H = h;

			m_caps = const_cast<bgfx::Caps*>(bgfx::getCaps());

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
		//- Set default backbuffer and state and display it



		//- Advance to next frame. Process submitted rendering primitives
		bgfx::frame();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::update_viewport(const vec2_t& position, const vec2_t& size)
	{
		S_X = (unsigned)position.x;
		S_Y = (unsigned)position.y;
		S_W = (unsigned)size.x;
		S_H = (unsigned)size.y;
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
		return false;
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