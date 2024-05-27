#include "spritemancer.hpp"
#include "detail/renderers/sm_renderer_raylib.hpp"
#include "detail/platforms/sm_platform_raylib.hpp"

namespace sm
{
	namespace
	{
		constexpr unsigned C_LAYER_COUNT_MAX = 256;
		static unsigned S_LAYER_COUNT = 0;
		static array_t<slayer, C_LAYER_COUNT_MAX> S_LAYERS;
		static core::cmutex S_MUTEX;

		static core::scolor S_WHITE = { 255, 255, 255, 255 };
		static core::scolor S_BLACK = { 0, 0, 0, 0 };
		static core::scolor S_BLANK = { 0, 0, 0, 255 };
		static std::atomic_bool S_RUNNING;
		static bool S_FULLSCREEN = false;
		static bool S_VSYNC = true;
		static unsigned S_X = 0, S_Y = 0, S_W = 0, S_H = 0;
		static float S_INVERSE_W = 0.0f, S_INVERSE_H = 0.0f;
		static float S_DT = 0.0f;
		static sblending S_BLEND_MODE_DEFAULT = { blending_mode_alpha, blending_equation_blend_color, blending_factor_src_color, blending_factor_one_minus_src_color };
		static sblending S_BLEND_MODE = S_BLEND_MODE_DEFAULT;

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
			entry::set_platform(std::make_unique<cplatform_raylib>());
			entry::set_renderer(std::make_unique<crenderer_raylib>());
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
			if (entry::platform()->optional_process_event() == opresult_fail)
			{
				S_RUNNING = false;
			}

			//- update HID state, such as keyboard and mouse

			entry::app()->on_update(S_DT);

			//- render frame
			entry::renderer()->update_viewport({ S_X, S_Y }, { S_W, S_H });

			//- most basic layer, does always exist
			S_LAYERS[0].m_show = true;
			entry::renderer()->prepare_frame();
			entry::renderer()->blendmode(S_BLEND_MODE);

			//- layered rendering, from bottom to top
			for (auto i = 0u; i < S_LAYER_COUNT; ++i)
			{
				auto& layer = S_LAYERS[i];

				if (layer.m_show &&
					entry::renderer()->begin(layer))
				{
					entry::renderer()->clear(layer, true);

					entry::renderer()->draw(layer);

					entry::renderer()->end(layer);
				}

				layer.m_commands.clear();
			}

			//- layered rendering, combine them upon each other
			for (auto i = 0u; i < S_LAYER_COUNT; ++i)
			{
				auto& layer = S_LAYERS[i];

				if (layer.m_show &&
					entry::renderer()->combine(layer))
				{
					//- do postprocess or whatever
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

			//- sample time for custom frame timing
		}

		//- Main engine thread where the update and rendering happens. Created from outside
		//------------------------------------------------------------------------------------------------------------------------
		void engine_thread()
		{
			//- starting up
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

			//- create with reasonable defaults
			layer.m_target.create(S_W, S_H);
			layer.m_combine_tint = S_WHITE;
			layer.m_clear_tint = S_WHITE;
			layer.m_flags = 0;
			layer.m_show = true;

			return S_LAYER_COUNT++;
		}
		return MAX(unsigned);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_line(unsigned layer, const vec2_t& start, const vec2_t& end, float thick, const core::scolor& color)
	{
		auto& command = S_LAYERS[layer].m_commands.emplace_back();

		command.create([=]()
			{
				raylib::DrawLineEx({ start.x, start.y }, { end.x, end.y }, thick, to_cliteral(color));
			});
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_circle(unsigned layer, const vec2_t& center, float radius, const core::scolor& color)
	{
		auto& command = S_LAYERS[layer].m_commands.emplace_back();

		command.create([=]()
			{
				raylib::DrawCircle(center.x, center.y, radius, to_cliteral(color));
			});
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_rect(unsigned layer, const vec2_t& position, const vec2_t& dimension, const core::scolor& color)
	{
		auto& command = S_LAYERS[layer].m_commands.emplace_back();

		command.create([=]()
			{
				raylib::DrawRectangleV({ position.x, position.y }, { dimension.x, dimension.y }, to_cliteral(color));
			});
	}

	//------------------------------------------------------------------------------------------------------------------------
	void set_logger(core::error_report_function_t callback)
	{
		serror_reporter::instance().m_callback = std::move(callback);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_placeholder(unsigned layer, const vec2_t& position, const vec2_t& scale /*= {1.0f, 1.0f}*/,
		const core::scolor& tint /*= {255, 255, 255, 255}*/)
	{
		draw_texture(layer, position, S_PLACEHOLDER_TEXTURE, scale, tint);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_texture(unsigned layer, const vec2_t& position, const srenderable& renderable, const vec2_t& scale, const core::scolor& tint)
	{
		if (algorithm::bit_on(renderable.m_flags, renderable_flag_invisible))
		{
			return;
		}

		auto& command = S_LAYERS[layer].m_commands.emplace_back();

		command.create([=]()
			{
				const auto w = renderable.m_texture.w();
				const auto h = renderable.m_texture.h();

				//- construct rectangles for where to sample from and where to draw
				raylib::Rectangle src = { renderable.m_src.x(), renderable.m_src.y(), renderable.m_src.w() * w, renderable.m_src.h() * h };
				raylib::Rectangle dst = { position.x, position.y, scale.x * w, scale.y * h };

				//- texture origin, i.e. the pivot point
				raylib::Vector2 origin = { 0.0f, 0.0f};

				//- check some flags and do adjustments
				if (algorithm::bit_on(renderable.m_flags, renderable_flag_origin_center))
				{
					origin = { w / 2.0f, h / 2.0f };
				}
				if (algorithm::bit_on(renderable.m_flags, renderable_flag_origin_custom))
				{
					origin = { renderable.m_origin.x, renderable.m_origin.y };
				}
				if (algorithm::bit_on(renderable.m_flags, renderable_flag_flipx))
				{
					src.width = -src.width;
				}
				if (algorithm::bit_on(renderable.m_flags, renderable_flag_flipy))
				{
					src.height = -src.height;
				}
				if (is_valid(renderable.m_shader))
				{
					raylib::BeginShaderMode(renderable.m_shader.shader());
				}
				if (algorithm::bit_on(renderable.m_flags, renderable_flag_blending_custom))
				{
					raylib::rlSetBlendMode(renderable.m_blending.m_mode);
					raylib::rlSetBlendFactors(renderable.m_blending.m_src_factor, renderable.m_blending.m_dst_factor, renderable.m_blending.m_equation);
					raylib::rlSetBlendMode(raylib::BLEND_CUSTOM);
				}

				raylib::DrawTexturePro(renderable.m_texture.texture(), src, dst, origin, renderable.m_rotation, to_cliteral(tint));

				//- reset state
				raylib::EndShaderMode();
			});
	}

} //- sm