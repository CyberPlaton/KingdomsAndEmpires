#include "sm_renderer_bgfx.hpp"

namespace sm
{
	namespace
	{
		static uint16_t S_W = 0;
		static uint16_t S_H = 0;
		static uint16_t S_X = 0;
		static uint16_t S_Y = 0;
		static unsigned S_BLENDING_MODE = BGFX_STATE_DEFAULT;

		constexpr unsigned C_STATE_DEFAULT = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_CULL_CCW | BGFX_STATE_MSAA;
		constexpr unsigned C_CLEAR_WITH_DEPTH = BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH;
		constexpr unsigned C_CLEAR_NO_DEPTH = BGFX_CLEAR_COLOR;
		constexpr bgfx::ViewId C_VIEW_DEFAULT = 0;
		constexpr mat4_t C_VIEWMAT_DEFAULT = mat4_t(1.0f);
		constexpr mat4_t C_PROJMAT_DEFAULT = mat4_t(1.0f);

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::prepare_device()
	{
		//- empty
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult crenderer_bgfx::init_device(void* nwh, unsigned w, unsigned h, bool fullscreen, bool vsync)
	{
		if (serror_reporter::instance().m_callback)
		{
			serror_reporter::instance().m_callback(core::logging_verbosity_info, "Initializing bgfx...");
		}

		//- on platforms that do not allow creating separate rendering threads call
		//- bgfx::renderFrame();

		S_W = w;
		S_H = h;

		bgfx::Init desc;
		desc.resolution.width = S_W;
		desc.resolution.height = S_H;
		desc.resolution.reset = vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE;
		desc.platformData.nwh = nwh;
#if CORE_PLATFORM_WINDOWS || CORE_PLATFORM_XBOXONE || CORE_PLATFORM_XBOXSERIES
		desc.type = bgfx::RendererType::Direct3D11;
#elif CORE_PLATFORM_LINUX
		desc.type = bgfx::RendererType::Vulkan;
#elif CORE_PLATFORM_OSX
		desc.type = bgfx::RendererType::Metal;
#elif CORE_PLATFORM_PS4 || CORE_PLATFORM_PS5
		desc.type = bgfx::RendererType::Gnm;
#elif CORE_PLATFORM_NX
		desc.type = bgfx::RendererType::Vulkan;
#endif

		//- report GPU capabilities and the selected configuration
		if (serror_reporter::instance().m_callback)
		{
			vector_t<bgfx::RendererType::Enum> renderers(bgfx::RendererType::Enum::Count);

			if (bgfx::getSupportedRenderers(bgfx::RendererType::Enum::Count, &renderers[0]) > 0)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_debug, "Supported renderers:");

				for (const auto& type : renderers)
				{
					//- ignore the noop rendering types, as there are usually several reported
					if (type != bgfx::RendererType::Enum::Noop)
					{
						serror_reporter::instance().m_callback(core::logging_verbosity_debug,
							fmt::format("\t'{}'", bgfx::getRendererName(type)));
					}
				}
			}

			serror_reporter::instance().m_callback(core::logging_verbosity_debug,
				fmt::format("Requested renderer:\n\t'{}'", bgfx::getRendererName(desc.type)));
		}

		//- try initializing bgfx
		if (!bgfx::init(desc))
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_critical, "Init bgfx failed...");
			}

			return opresult_fail;
		}

		//- report success and continue reporting capabilities. Implement if required
		//const auto* caps = bgfx::getCaps();

#if DEBUG
		bgfx::setDebug(BGFX_DEBUG_TEXT | BGFX_DEBUG_STATS);
#endif

		return opresult_ok;
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
		bgfx::touch(C_VIEW_DEFAULT);
		bgfx::setViewRect(C_VIEW_DEFAULT, S_X, S_Y, S_W, S_H);
		bgfx::setViewTransform(C_VIEW_DEFAULT, glm::value_ptr(C_VIEWMAT_DEFAULT), glm::value_ptr(C_PROJMAT_DEFAULT));
		bgfx::setState(C_STATE_DEFAULT);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::display_frame()
	{
		//- show some debug stats
		const bgfx::Stats* stats = bgfx::getStats();

		bgfx::dbgTextClear();
		bgfx::dbgTextPrintf(0, 2, 0x0f, "Backbuffer %dW x %dH in pixels, debug text %dW x %dH in characters.",
			stats->width, stats->height, stats->textWidth, stats->textHeight);

		bgfx::ProgramHandle invalid; invalid.idx = bgfx::kInvalidHandle;

		bgfx::submit(C_VIEW_DEFAULT, invalid);
		bgfx::frame();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::update_viewport(const vec2_t& position, const vec2_t& size)
	{
		S_X = (uint16_t)position.x;
		S_Y = (uint16_t)position.y;
		S_W = (uint16_t)size.x;
		S_H = (uint16_t)size.y;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::clear(const core::scolor& color, bool depth)
	{
		bgfx::setViewClear(C_VIEW_DEFAULT, depth ? C_CLEAR_WITH_DEPTH : C_CLEAR_NO_DEPTH, color.rgba(), 1.0f, 0);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::blendmode(blending_mode mode)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::bind_texture(uint64_t id)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::render_layer_quad(const vec2_t& position, const vec2_t& size, const core::scolor& color)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::render_decal(const sdecal& decal)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::update_texture_gpu(uint64_t id, unsigned w, unsigned h, void* data)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::update_texture_cpu(uint64_t id)
	{

	}

} //- sm