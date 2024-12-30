#include "sm_renderer_bgfx.hpp"
#include "../sm_embedded_shaders.hpp"
#include "../sm_renderpass.hpp"
#include "../sm_context.hpp"

RTTR_REGISTRATION
{
	using namespace sm;

	rttr::cregistrator<crenderer_bgfx>("crenderer_bgfx")
		;
}

namespace sm
{
	namespace shaders
	{
		constexpr stringview_t C_VERTEX_DEFAULT =
			"$input a_position, a_color0, a_texcoord0					\n"
			"$output v_color0, v_texcoord0								\n"
			"#include <bgfx_shader.sh>									\n"
			"void main()												\n"
			"{															\n"
			"	gl_Position = mul(u_viewProj, vec4(a_position, 1.0));	\n"
			"	v_color0 = a_color0;									\n"
			"	v_texcoord0 = a_texcoord0;								\n"
			"};															\n"
			"\0"
			;

		constexpr stringview_t C_FRAGMENT_DEFAULT =
			"$input v_color0, v_texcoord0								\n"
			"#include <bgfx_shader.sh>									\n"
			"SAMPLER2D(s_tex, 0);										\n"
			"void main()												\n"
			"{															\n"
			"	vec4 color = v_color0 * texture2D(s_tex, v_texcoord0);	\n"
			"	if (color.w < 1.0/255.0) { discard; }					\n"
			"	gl_FragColor = color;									\n"
			"};															\n"
			"\0"
			;

	} //- shaders

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

		bgfx::Init desc;
		desc.resolution.width = w;
		desc.resolution.height = h;
		desc.resolution.reset = vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE;
		desc.platformData.nwh = nwh;
#if CORE_DEBUG
		desc.debug = true;
#else
		desc.debug = false;
#endif
#if TRACY_ENABLE
		desc.profile = true;
#else
		desc.profile = false;
#endif

#if CORE_PLATFORM_WINDOWS || CORE_PLATFORM_XBOXONE || CORE_PLATFORM_XBOXSERIES
		desc.type = bgfx::RendererType::Direct3D11;
		desc.platformData.ndt = nullptr;
#elif CORE_PLATFORM_LINUX
		desc.type = bgfx::RendererType::Vulkan;
		desc.platformData.ndt = XOpenDisplay(nullptr);
#elif CORE_PLATFORM_OSX
		desc.type = bgfx::RendererType::Metal;
		desc.platformData.ndt = nullptr;
#elif CORE_PLATFORM_PS4 || CORE_PLATFORM_PS5
		desc.type = bgfx::RendererType::Gnm;
		desc.platformData.ndt = nullptr;
#elif CORE_PLATFORM_NX
		desc.type = bgfx::RendererType::Vulkan;
		desc.platformData.ndt = nullptr;
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
		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult crenderer_bgfx::shutdown_device()
	{
		bgfx::shutdown();

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::begin()
	{
		const auto& gfx = entry::ctx()->graphics();

		vector_t<renderpass_id_t> order(gfx.m_renderpass_order.size());

		for (const auto& pair : gfx.m_renderpass_order)
		{
			order.emplace_back(pair.first);
		}

		bgfx::setViewOrder(0, SCAST(uint16_t, order.size()), order.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::end()
	{
		bgfx::frame();
	}

} //- sm