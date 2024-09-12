#include "sm_renderer_bgfx.hpp"

namespace sm
{
	namespace
	{
		static ccamera S_CAMERA;
		static unsigned S_X = 0, S_Y = 0, S_W = 0, S_H = 0;

		//------------------------------------------------------------------------------------------------------------------------
		void screen_space_quad(bool _originBottomLeft, float _width = 1.0f, float _height = 1.0f)
		{
			if (3 == bgfx::getAvailTransientVertexBuffer(3, vertex_layouts::spostexcoord::S_LAYOUT))
			{
				bgfx::TransientVertexBuffer vb;
				bgfx::allocTransientVertexBuffer(&vb, 3, vertex_layouts::spostexcoord::S_LAYOUT);

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

				cvertices vertices(vertex_layouts::spostexcoord::S_LAYOUT, 3);
				vertices
					.begin(0)
					.position({ minx, miny, zz })
					.tex_coord({ minu, minv })
					.begin(1)
					.position({ maxx, miny, zz })
					.tex_coord({ maxu, minv })
					.begin(2)
					.position({ maxx, maxy, zz })
					.tex_coord({ maxu, maxv })
					.end()
					;

				bgfx::setVertexBuffer(0, &vb);
			}
		}

	} //- unnamed

	//- Things to be done before we initialize bgfx
	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::prepare_device()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult crenderer_bgfx::init_device(void* nwh, unsigned w, unsigned h, bool fullscreen, bool vsync)
	{
		bgfx::Init config;

#if CORE_PLATFORM_WINDOWS
		config.type = bgfx::RendererType::Direct3D11;
#else
		config.type = bgfx::RendererType::OpenGL;
#endif
		config.resolution.width = w;
		config.resolution.height = h;
		config.resolution.reset = (vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE) |
			(fullscreen ? BGFX_RESET_FULLSCREEN : BGFX_RESET_NONE);
		config.platformData.nwh = nwh;

		//- In case we want to use single-threaded rendering call this once
		//- bgfx::renderFrame();

		if (bgfx::init(config))
		{
			S_W = w;
			S_H = h;

			//- initialize vertex types
			auto result = true;

			result &= vertex_layouts::spostexcoord::init();
			result &= vertex_layouts::spostexcoordcolor::init();

			CORE_ASSERT(result, "Invalid operation. Failed to initialize one or more vertex layouts!");

			m_caps = const_cast<bgfx::Caps*>(bgfx::getCaps());

			return result ? opresult_ok : opresult_fail;
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
	void crenderer_bgfx::on_event(const rttr::variant& event)
	{
		if (event.is_type<events::window::sresize>())
		{
			const auto& e = event.convert<events::window::sresize>();

			//- Main window was resized, so we can set position of backbuffer start at 0
			update_viewport({0.0f, 0.0f}, {(float)e.w, (float)e.h});
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::update_viewport(const vec2_t& position, const vec2_t& size)
	{
		const auto x = (unsigned)position.x;
		const auto y = (unsigned)position.y;
		const auto w = (unsigned)size.x;
		const auto h = (unsigned)size.y;

		if (S_X != x || S_Y != y || S_W != w || S_H != h)
		{
			S_X = x;
			S_Y = y;
			S_W = w;
			S_H = h;

			S_CAMERA.m_viewport.m_x = S_X;
			S_CAMERA.m_viewport.m_y = S_Y;
			S_CAMERA.m_viewport.m_w = S_W;
			S_CAMERA.m_viewport.m_h = S_H;
			S_CAMERA.matrix_projection_update();

			bgfx::reset(w, h);
			bgfx::setViewRect(0, S_X, S_Y, S_W, S_H);
		}
	}

	//- Set state, camera data and framebuffer defined in layer
	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::frame_begin()
	{
		//- This dummy draw call is here to make sure that view 0 is cleared if no other draw calls are submitted to view 0
		bgfx::touch(0);

		//- setup default view
		S_CAMERA.matrix_view_update();

		bgfx::setViewClear(0, S_CAMERA.clear_state(), S_CAMERA.clearcolor().rgba(), 1.0f, 0);
		bgfx::setViewRect(0, S_CAMERA.viewport().x(), S_CAMERA.viewport().y(), S_CAMERA.viewport().w(), S_CAMERA.viewport().h());
		bgfx::setViewTransform(0, glm::value_ptr(S_CAMERA.matrix_view()), glm::value_ptr(S_CAMERA.matrix_projection()));
	}

	//- Reset state, camera data and framebuffer
	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::frame_end()
	{
		//- Set default backbuffer and state and display it
		bgfx::setViewFrameBuffer(0, { bgfx::kInvalidHandle });

		// Advance to next frame. Process submitted rendering primitives
		bgfx::frame();
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