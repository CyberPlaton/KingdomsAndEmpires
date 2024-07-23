#include "spritemancer.hpp"
#include "detail/os/sm_os_headless.hpp"
#include "detail/renderers/sm_renderer_headless.hpp"
#include "detail/renderers/sm_renderer_raylib.hpp"
#include "detail/os/sm_os_raylib.hpp"
#include "detail/sm_resource_manager.hpp"
#include "detail/sm_embedded_shaders.hpp"

namespace sm
{
	namespace
	{
		constexpr unsigned C_LAYER_COUNT_MAX = 256;
		static unsigned S_LAYER_COUNT = 0;
		static array_t<slayer, C_LAYER_COUNT_MAX> S_LAYERS;
		static core::cmutex S_MUTEX;

		static shader_handle_t S_DEFAULT_SHADER = 0;
		static core::scolor S_WHITE = { 255, 255, 255, 255 };
		static core::scolor S_BLACK = { 0, 0, 0, 0 };
		static core::scolor S_BLANK = { 0, 0, 0, 255 };
		static std::atomic_bool S_RUNNING;
		static bool S_FULLSCREEN = false;
		static bool S_VSYNC = true;
		static unsigned S_X = 0, S_Y = 0, S_W = 0, S_H = 0;
		static bool S_RESIZE_REQUIRED = false;
		static float S_DT = 0.0f;
		static texture_handle_t S_PLACEHOLDER_TEXTURE = 0;
		static sblending S_BLEND_MODE_DEFAULT = { blending_mode_alpha, blending_equation_blend_color, blending_factor_src_color, blending_factor_one_minus_src_color };
		static srenderstate S_RENDERSTATE_DEFAULT = { S_BLEND_MODE_DEFAULT, 0 };
		constexpr float C_ROTATION_DEFAULT = 0.0f;
		constexpr vec2_t C_SCALE_DEFAULT = {1.0f, 1.0f};
		constexpr vec2_t C_ORIGIN_DEFAULT = {0.5f, 0.5f};
		static inline const core::srect C_SOURCE_DEFAULT = {0.0f, 0.0f, 1.0f, 1.0f};

		//- current used rendering state data
		static sblending S_CURRENT_BLEND_MODE = S_BLEND_MODE_DEFAULT;
		static srenderstate S_CURRENT_RENDERSTATE = S_RENDERSTATE_DEFAULT;
		static shader_handle_t S_CURRENT_SHADER = S_DEFAULT_SHADER;

		static void* S_CONFIG = nullptr;
		static argparse::ArgumentParser S_ARGS;

		//- Load some default assets such as default shaders, images and textures etc.
		//------------------------------------------------------------------------------------------------------------------------
		void load_internal_resources()
		{
			S_DEFAULT_SHADER = core::cservice_manager::find<cshader_manager>()->load_sync("sprite", shader_type_fragment, shaders::sprite::C_VS, shaders::sprite::C_PS);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void update_window_size(unsigned w, unsigned h)
		{
			S_W = w;
			S_H = h;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void update_window_viewport()
		{
			entry::get_renderer()->update_viewport({ S_X, S_Y }, { S_W, S_H });
		}

		//------------------------------------------------------------------------------------------------------------------------
		void engine_configure_platform_and_renderer(iapp* app)
		{
			entry::set_app(app);
			entry::set_os(std::make_unique<cos_raylib>());
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
			if (entry::get_os()->optional_process_event() == opresult_fail)
			{
				S_RUNNING = false;
			}

			//- check for resize of most basic layer. Other layers are not our responsibility
			if (S_RESIZE_REQUIRED)
			{
				S_LAYERS[0].m_target.resize(S_W, S_H);

				S_RESIZE_REQUIRED = false;
			}

			//- update HID state, such as keyboard and mouse

			entry::get_app()->on_update(S_DT);

			//- render frame
			entry::get_renderer()->update_viewport({ S_X, S_Y }, { S_W, S_H });

			//- most basic layer, does always exist
			S_LAYERS[0].m_show = true;
			entry::get_renderer()->prepare_frame();
			entry::get_renderer()->blendmode(S_BLEND_MODE_DEFAULT);

			//- layered rendering, from bottom to top
			for (auto i = 0u; i < S_LAYER_COUNT; ++i)
			{
				auto& layer = S_LAYERS[i];

				if (layer.m_show &&
					entry::get_renderer()->begin(layer))
				{
					entry::get_renderer()->clear(layer, true);

					entry::get_renderer()->draw(layer);

					entry::get_renderer()->end(layer);
				}

				layer.m_commands.clear();
			}

			//- layered rendering, combine them upon each other
			for (auto i = 0u; i < S_LAYER_COUNT; ++i)
			{
				auto& layer = S_LAYERS[i];

				if (layer.m_show &&
					entry::get_renderer()->combine(layer))
				{
					//- do postprocess or whatever
				}
			}

			//- finalize rendering with imgui on top
			entry::get_app()->on_imgui();

			//- present everything
			entry::get_renderer()->display_frame();
		}

		//------------------------------------------------------------------------------------------------------------------------
		void engine_prepare()
		{
			update_window_size(S_W, S_H);
			update_window_viewport();

			//- create resource managers
			core::cservice_manager::emplace<cimage_manager>();
			core::cservice_manager::emplace<ctexture_manager>();
			core::cservice_manager::emplace<cshader_manager>();
			core::cservice_manager::emplace<cspriteatlas_manager>();
			core::cservice_manager::emplace<crendertarget_manager>();

			load_internal_resources();

			//- create default placeholder texture
			{
				cimage solid([]() -> raylib::Image
					{
						return raylib::GenImageColor(256, 256, S_WHITE.cliteral<raylib::Color>());
					});

				S_PLACEHOLDER_TEXTURE = core::cservice_manager::find<ctexture_manager>()->load_sync("placeholder", solid);

				cimage::destroy(solid);
			}

			//- create default font

			//- create default rendering layer
			create_layer();

			//- sample time for custom frame timing
		}

		//------------------------------------------------------------------------------------------------------------------------
		void engine_finalize_init()
		{
			//- initialize imgui
			imgui::init();

			//- create internal event listeners
			core::cservice_manager::find<core::cevent_service>()->emplace_listener<events::window::sresize>([](const rttr::variant& var)
				{
					const auto& e = var.convert<events::window::sresize>();
					S_W = e.w;
					S_H = e.h;

					S_RESIZE_REQUIRED = true;
				});
		}

		//- Main engine thread where the update and rendering happens. Created from outside
		//------------------------------------------------------------------------------------------------------------------------
		void engine_thread()
		{
			//- main loop
			while (S_RUNNING)
			{
				engine_update();
			}

			//- shutting down
			entry::get_app()->on_shutdown();
			imgui::shutdown();
			if (entry::has_platform()) { entry::get_platform()->shutdown(); }
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
		if (entry::has_platform() && entry::get_platform()->pre_init() != opresult_ok)
		{
			return opresult_fail;
		}

		if (entry::get_os()->init() != opresult_ok)
		{
			return opresult_fail;
		}

		if (entry::get_os()->init_mainwindow(title.data(), w, h, fullscreen) != opresult_ok)
		{
			return opresult_fail;
		}

		if (entry::has_platform() && entry::get_platform()->init() != opresult_ok)
		{
			return opresult_fail;
		}

		//- cache common data
		entry::get_os()->main_window_position(&S_X, &S_Y);
		entry::get_os()->main_window_size(&S_W, &S_H);
		S_FULLSCREEN = fullscreen;
		S_VSYNC = vsync;

		//- starting up graphics and app
		if (entry::get_os()->init_gfx(S_W, S_H, S_FULLSCREEN, S_VSYNC) != opresult_ok)
		{
			return opresult_fail;
		}

		engine_prepare();

		//- TODO: ignore return value for testing and development
		entry::get_app()->on_init(S_CONFIG, S_ARGS);
// 		if (entry::get_app()->on_init(S_CONFIG, S_ARGS))
// 		{
// 			return opresult_fail;
// 		}

		engine_finalize_init();

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult run()
	{
		S_RUNNING = true;

		engine_thread();

		if (entry::get_os()->shutdown() != opresult_ok)
		{
			return opresult_fail;
		}

		if (entry::has_platform()) { entry::get_platform()->post_shutdown(); }

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
		SM_DRAW_CALL(6); //- 6 vertices? Really???

		auto& command = S_LAYERS[layer].m_commands.emplace_back();

		command.create([=]()
			{
				raylib::DrawLineEx({ start.x, start.y }, { end.x, end.y }, thick, to_cliteral(color));
			});
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_circle(unsigned layer, const vec2_t& center, float radius, const core::scolor& color)
	{
		SM_DRAW_CALL(4);

		auto& command = S_LAYERS[layer].m_commands.emplace_back();

		command.create([=]()
			{
				raylib::DrawCircle(center.x, center.y, radius, to_cliteral(color));
			});
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_rect(unsigned layer, const vec2_t& position, const vec2_t& dimension, const core::scolor& color)
	{
		SM_DRAW_CALL(4);

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
		draw_texture(layer, position, S_PLACEHOLDER_TEXTURE, tint, C_ROTATION_DEFAULT, scale);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture)
	{
		draw_texture(layer, position, texture, S_WHITE);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture, const core::scolor& tint)
	{
		draw_texture(layer, position, texture, tint, C_ROTATION_DEFAULT);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture, const core::scolor& tint, float rotation)
	{
		draw_texture(layer, position, texture, tint, rotation, C_SCALE_DEFAULT);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture, const core::scolor& tint, float rotation,
		const vec2_t& scale)
	{
		draw_texture(layer, position, texture, tint, rotation, scale, S_DEFAULT_SHADER);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture, const core::scolor& tint, float rotation,
		const vec2_t& scale, shader_handle_t shader)
	{
		draw_texture(layer, position, texture, tint, rotation, scale, shader, S_RENDERSTATE_DEFAULT);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture, const core::scolor& tint, float rotation,
		const vec2_t& scale, shader_handle_t shader, const srenderstate& state)
	{
		draw_texture(layer, position, texture, tint, rotation, scale, shader, state, C_ORIGIN_DEFAULT);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture, const core::scolor& tint, float rotation,
		const vec2_t& scale, shader_handle_t shader, const srenderstate& state, const vec2_t& origin)
	{
		draw_texture(layer, position, texture, tint, rotation, scale, shader, state, origin, C_SOURCE_DEFAULT);
	}

	//- The actual rendering routine for a texture/sprite
	//------------------------------------------------------------------------------------------------------------------------
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture, const core::scolor& tint, float rotation,
		const vec2_t& scale, shader_handle_t shader, const srenderstate& state, const vec2_t& origin, const core::srect& source)
	{
		SM_DRAW_CALL(4);

		if (algorithm::bit_check(state.m_flags, renderable_flag_invisible))
		{
			return;
		}

		//- create a sprite draw command
		{
			core::cscope_mutex m(S_MUTEX);

			S_LAYERS[layer].m_commands.emplace_back().create([=]()
				{
					//- TODO: think of a better way to get actual shaders, textures etc. from resource managers,
					//- Note: these command are executed on main thread, but other threads may still post load/unload
					//- requests, so we have to account for that and make them thread-safe.
					const auto& tm = *core::cservice_manager::find<ctexture_manager>();
					const auto& sm = *core::cservice_manager::find<cshader_manager>();

					//- texture and dimension
					const auto& _texture = tm.at(texture);
					const auto w = _texture.w();
					const auto h = _texture.h();

					//- construct rectangles for where to sample from and where to draw
					raylib::Rectangle src = { source.x(), source.y(), source.w() * w, source.h() * h };
					raylib::Rectangle dst = { position.x, position.y, scale.x * w, scale.y * h };
					raylib::Vector2 orig = { origin.x, origin.y };

					//- check some flags and do adjustments
					if (algorithm::bit_check(state.m_flags, renderable_flag_origin_center))
					{
						orig = { w * 0.5f, h * 0.5f };
					}
					if (algorithm::bit_check(state.m_flags, renderable_flag_origin_custom))
					{
						orig = { origin.x, origin.y };
					}
					if (algorithm::bit_check(state.m_flags, renderable_flag_flipx))
					{
						src.width = -src.width;
					}
					if (algorithm::bit_check(state.m_flags, renderable_flag_flipy))
					{
						src.height = -src.height;
					}

					//- check if shader has changed since last draw command
					if (shader != S_CURRENT_SHADER)
					{
						S_CURRENT_SHADER = shader;
					}

					//- setting shader is always enabled as we set default shader manually
					const auto& _shader = sm.at(S_CURRENT_SHADER);
					raylib::BeginShaderMode(_shader.shader());

					//- check if renderstate has changed since last draw command
					if (state != S_CURRENT_RENDERSTATE)
					{
						S_CURRENT_RENDERSTATE = state;
					}

					//- set blending mode only if enabled for this draw command
					if (algorithm::bit_check(state.m_flags, renderable_flag_blending_custom))
					{
						raylib::rlSetBlendMode(S_CURRENT_RENDERSTATE.m_blending.m_mode);
						raylib::rlSetBlendFactors(S_CURRENT_RENDERSTATE.m_blending.m_src_factor, S_CURRENT_RENDERSTATE.m_blending.m_dst_factor, S_CURRENT_RENDERSTATE.m_blending.m_equation);
						raylib::rlSetBlendMode(raylib::BLEND_CUSTOM);
					}

					raylib::DrawTexturePro(_texture.texture(), src, dst, orig, rotation, tint.cliteral<raylib::Color>());
				});
		}
	}

} //- sm