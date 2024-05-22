#include "spritemancer.hpp"
#include "detail/renderers/sm_renderer_bgfx.hpp"
#include "detail/platforms/sm_platform_sdl2.hpp"

namespace sm
{
	namespace
	{
		constexpr unsigned C_LAYER_COUNT_MAX = 256;
		static unsigned S_LAYER_COUNT = 0;
		static unsigned S_CURRENT_LAYER = 0;
		static array_t<slayer, C_LAYER_COUNT_MAX> S_LAYERS;

		static core::scolor S_WHITE = { 255, 255, 255, 255 };
		static core::scolor S_BLACK = { 0, 0, 0, 0 };
		static core::scolor S_BLANK = { 0, 0, 0, 255 };
		static std::atomic_bool S_RUNNING;
		static bool S_FULLSCREEN = false;
		static bool S_VSYNC = true;
		static unsigned S_X = 0, S_Y = 0, S_W = 0, S_H = 0;
		static float S_INVERSE_W = 0.0f, S_INVERSE_H = 0.0f;
		static float S_DT = 0.0f;
		static blending_mode S_BLEND_MODE = blending_mode_default;
		static primitive_topology_t S_TOPOLOGY = primitive_topology_t::TriList;

		static void* S_CONFIG = nullptr;
		static argparse::ArgumentParser S_ARGS;

		static srenderable S_PLACEHOLDER_TEXTURE;

		//------------------------------------------------------------------------------------------------------------------------
		void update_window_size(unsigned w, unsigned h)
		{
			S_W = w;
			S_H = h;
			S_INVERSE_W = 1.0f / (float)S_W;
			S_INVERSE_H = 1.0f / (float)S_H;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void update_window_viewport()
		{
			entry::renderer()->update_viewport({ S_X, S_Y }, { S_W, S_H });
		}

		//------------------------------------------------------------------------------------------------------------------------
		void engine_configure_platform_and_renderer(iapp* app)
		{
			entry::set_app(app);
			entry::set_platform(std::make_unique<cplatform_sdl>());
			entry::set_renderer(std::make_unique<crenderer_bgfx>());
		}

		//------------------------------------------------------------------------------------------------------------------------
		void engine_configure_args()
		{
			//- common arguments available everywhere
			S_ARGS.add_argument("--world")
				.default_value("");

			//- configuration specific arguments
#if DEBUG
			S_ARGS.add_argument("--console")
				.default_value(true);
#else
#endif
		}

		//------------------------------------------------------------------------------------------------------------------------
		void engine_update()
		{
			CORE_ZONE;

			//- platforms may need to handle events
			if (entry::platform()->process_event() == opresult_fail)
			{
				S_RUNNING = false;
			}

			//- update HID state, such as keyboard and mouse

			entry::app()->on_update(S_DT);

			//- render frame
			entry::renderer()->update_viewport({ S_X, S_Y }, { S_W, S_H });

			//- most basic layer, does always exist
			S_LAYERS[0].m_want_update = true;
			S_LAYERS[0].m_show = true;
			entry::renderer()->blendmode(blending_mode_default);
			entry::renderer()->prepare_frame();

			//- layered rendering, from bottom to top
			for (auto i = 0u; i < S_LAYER_COUNT; ++i)
			{
				auto& layer = S_LAYERS[i];

				if (layer.m_show)
				{
					entry::renderer()->clear(i, layer.m_tint, true);

					//- TODO: unclear what this was originally for. Clearing the layer, however, should be
					//- done as shown above
					//- bind render target texture and fill it with color
// 					entry::renderer()->bind_texture(layer.m_layer_target.m_texture.handle().idx);
// 					if (layer.m_want_update)
// 					{
// 						entry::renderer()->update_texture_gpu(layer.m_layer_target.m_texture.handle().idx,
// 							layer.m_layer_target.m_image.image()->m_width,
// 							layer.m_layer_target.m_image.image()->m_height,
// 							layer.m_layer_target.m_image.image()->m_data);
// 
// 						layer.m_want_update = false;
// 					}

					//entry::renderer()->render_layer_quad({ S_X, S_Y }, { S_W, S_H }, layer.m_tint);

					//- render decals/textures for current layer
					for (const auto& decal : layer.m_decals)
					{
						entry::renderer()->render_decal(decal);
					}
					layer.m_decals.clear();
				}
			}

			//- present everything
			entry::renderer()->display_frame();
		}

		//------------------------------------------------------------------------------------------------------------------------
		void engine_prepare()
		{
			update_window_size(S_W, S_H);
			update_window_viewport();

			//- create default placeholder texture
			S_PLACEHOLDER_TEXTURE.m_image.create_checkerboard(256, 256, 16, S_WHITE, S_BLACK);
			S_PLACEHOLDER_TEXTURE.m_texture.load_from_image(S_PLACEHOLDER_TEXTURE.m_image);

			//- create default font

			//- create default rendering layer
			create_layer();
			S_LAYERS[0].m_show = true;
			S_LAYERS[0].m_want_update = true;
			bind_layer(0);

			//- sample time for custom frame timing
		}

		//- Main engine thread where the update and rendering happens. Created from outside
		//------------------------------------------------------------------------------------------------------------------------
		void engine_thread()
		{
			//- starting up
			if (entry::platform()->init_on_thread() != opresult_ok)
			{
				return;
			}
			if (entry::platform()->init_gfx(S_W, S_H, S_FULLSCREEN, S_VSYNC) != opresult_ok)
			{
				return;
			}

			engine_prepare();

			if (entry::app()->on_init(S_CONFIG, S_ARGS))
			{
				S_RUNNING = false;
			}

			//- main loop
			while (S_RUNNING)
			{
				engine_update();
			}

			//- shutting down
			entry::app()->on_shutdown();
			entry::platform()->shutdown_on_thread();
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult configure(iapp* app, void* config, int argc, char* argv[])
	{
		S_CONFIG = config;

		engine_configure_platform_and_renderer(app);

		engine_configure_args();

		//- argparse throws on errorss
		try
		{
			if (argc > 0 && argv)
			{
				S_ARGS.parse_args(argc, argv);
			}
		}
		catch (const std::runtime_error& err)
		{
			return opresult_fail;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult init(stringview_t title, unsigned w, unsigned h, bool fullscreen, bool vsync)
	{
		if (entry::platform()->init() != opresult_ok)
		{
			return opresult_fail;
		}
		if (entry::platform()->init_mainwindow(title.data(), w, h, fullscreen) != opresult_ok)
		{
			return opresult_fail;
		}

		//- cache common data
		entry::platform()->main_window_position(&S_X, &S_Y);
		entry::platform()->main_window_size(&S_W, &S_H);
		S_FULLSCREEN = fullscreen;
		S_VSYNC = vsync;

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult run()
	{
		S_RUNNING = true;

		entry::platform()->optional_init_event_mainloop();

		engine_thread();

		if (entry::platform()->shutdown() != opresult_ok)
		{
			return opresult_fail;
		}
		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned create_layer()
	{
		if (S_LAYER_COUNT < C_LAYER_COUNT_MAX)
		{
			auto& layer = S_LAYERS[S_LAYER_COUNT];

			layer.m_layer_target.m_image.create_solid(S_W, S_H, S_BLANK);
			layer.m_layer_target.m_texture.load_from_image(layer.m_layer_target.m_image);
			layer.m_tint = S_BLANK;

			return S_LAYER_COUNT++;
		}
		return MAX(unsigned);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool bind_layer(unsigned i)
	{
		if (i >= 0 && i <= S_LAYER_COUNT)
		{
			S_CURRENT_LAYER = i;
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void sm_logger(core::error_report_function_t callback)
	{
		serror_reporter::instance().m_callback = std::move(callback);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_placeholder(const vec2_t& position, const vec2_t& scale /*= {1.0f, 1.0f}*/,
		const core::scolor& tint /*= {255, 255, 255, 255}*/)
	{
		draw_texture(position, S_PLACEHOLDER_TEXTURE, scale, tint);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_texture(const vec2_t& position, const srenderable& renderable, const vec2_t& scale, const core::scolor& tint)
	{
		vec2_t vScreenSpacePos =
		{
			(position.x * S_INVERSE_W) * 2.0f - 1.0f,
			((position.y * S_INVERSE_H) * 2.0f - 1.0f) * -1.0f
		};

		vec2_t vScreenSpaceDim =
		{
			vScreenSpacePos.x + (2.0f * (float(renderable.m_image.image()->m_width) * S_INVERSE_W)) * scale.x,
			vScreenSpacePos.y - (2.0f * (float(renderable.m_image.image()->m_height) * S_INVERSE_H)) * scale.y
		};

		auto& decal = S_LAYERS[S_CURRENT_LAYER].m_decals.emplace_back();
		decal.m_texture = renderable.m_texture.handle();
		decal.m_points = 4;
		decal.m_tint = { tint, tint, tint, tint };
		decal.m_position = { { vScreenSpacePos.x, vScreenSpacePos.y }, { vScreenSpacePos.x, vScreenSpaceDim.y }, { vScreenSpaceDim.x, vScreenSpaceDim.y }, { vScreenSpaceDim.x, vScreenSpacePos.y } };
		decal.m_uv = { { 0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f} };
		decal.m_w = { 1, 1, 1, 1 };
		decal.m_blending = S_BLEND_MODE;
		decal.m_topology = S_TOPOLOGY;
	}

} //- sm