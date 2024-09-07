#include "sm_renderer_bgfx.hpp"
#include "../sm_vertices.hpp"

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

	//- Things to be done before we initialize bgfx
	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::prepare_device()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult crenderer_bgfx::init_device(void* nwh, unsigned w, unsigned h, bool fullscreen, bool vsync)
	{
		bgfx::Init config;
		config.type = bgfx::RendererType::Vulkan;
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
	void crenderer_bgfx::prepare_frame()
	{
		//- This dummy draw call is here to make sure that view 0 is cleared if no other draw calls are submitted to view 0
		bgfx::touch(0);

		//- setup default view
		S_CAMERA.matrix_view_update();

		bgfx::setViewClear(0, S_CAMERA.clear_state(), S_CAMERA.clearcolor().rgba(), 1.0f, 0);
		bgfx::setViewRect(0, S_CAMERA.viewport().x(), S_CAMERA.viewport().y(), S_CAMERA.viewport().w(), S_CAMERA.viewport().h());
		bgfx::setViewTransform(0, glm::value_ptr(S_CAMERA.matrix_view()), glm::value_ptr(S_CAMERA.matrix_projection()));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::display_frame()
	{
		//- Set default backbuffer and state and display it

#if CORE_DEBUG
		bgfx::setDebug(BGFX_DEBUG_TEXT);
		const bgfx::Stats* stats = bgfx::getStats();
		const auto os = entry::get_os();
		double mx, my, sx, sy;
		os->mouse_position(&mx, &my);
		os->mouse_scroll_dt(&sx, &sy);

		bgfx::dbgTextClear();
		bgfx::dbgTextPrintf(0, 1, 0x0f, fmt::format("Window: '{}:{}:{}:{}'", S_X, S_Y, S_W, S_H).data());
		bgfx::dbgTextPrintf(0, 2, 0x0f, fmt::format("Mouse: '{}:{}'", mx, my).data());
		bgfx::dbgTextPrintf(0, 3, 0x0f, fmt::format("Scroll: '{}:{}'", sx, sy).data());

		bgfx::dbgTextPrintf(0, 4, 0x0f, "Backbuffer %dW x %dH in pixels. CPU: %.4f ms          GPU: %.4f ms"
			, stats->width
			, stats->height
			, double(stats->cpuTimeEnd - stats->cpuTimeBegin) * 1000.0 / stats->cpuTimerFreq
			, double(stats->gpuTimeEnd - stats->gpuTimeBegin) * 1000.0 / stats->gpuTimerFreq
		);
#endif

		// Advance to next frame. Process submitted rendering primitives
		bgfx::frame();
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

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::blendmode(sblending mode)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::clear(const slayer& layer, bool depth)
	{

	}

	//- Set state, camera data and framebuffer defined in layer
	//------------------------------------------------------------------------------------------------------------------------
	bool crenderer_bgfx::begin(const slayer& layer)
	{
		if (is_valid(layer.m_target))
		{
			auto& view = m_views[layer.m_id];
			view.id() = static_cast<uint16_t>(layer.m_id);
			view.target() = layer.m_target;
			view.camera() = layer.m_camera;
			view.program() = layer.m_program;

			bgfx::setViewClear(view.id(), view.camera().clear_state(),
				view.camera().clearcolor().rgba());

			const auto& rect = view.camera().viewport();

			bgfx::setViewRect(view.id(), rect.m_x, rect.m_y, rect.m_w, rect.m_h);

			bgfx::setViewFrameBuffer(view.id(), view.target().target());

			//- Check some flags and do adjustments
			if (algorithm::bit_check(layer.m_flags, layer_flags_2d))
			{
				//- Push orthogonal camera transform data
				bgfx::setViewTransform(view.id(),
					glm::value_ptr(view.camera().matrix_view()),
					glm::value_ptr(view.camera().matrix_projection()));
			}

			bgfx::setState(view.camera().state(), view.camera().clearcolor().rgba());

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

	//- Reset state, camera data and framebuffer
	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::end(const slayer& layer)
	{
		auto& view = m_views[layer.m_id];

		bgfx::setViewFrameBuffer(view.id(), { bgfx::kInvalidHandle });
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool crenderer_bgfx::combine(const slayer& layer)
	{
		auto& view = m_views[layer.m_id];

		//- Draw the target texture from layer on top backbuffer
		if (is_valid(view.target()))
		{
			screen_space_quad(m_caps->originBottomLeft, layer.m_scale.x, layer.m_scale.y);

			bgfx::setTexture(0, view.target().uniform().uniform(), view.target().texture());

			bgfx::submit(0, view.program().program());

			return true;
		}
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