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
		static void* S_CONFIG				= nullptr;
		static argparse::ArgumentParser S_ARGS;
		static core::cmutex S_MUTEX;
		static std::atomic_bool S_RUNNING;
		static bool S_FULLSCREEN			= false;
		static bool S_VSYNC					= true;
		static bool S_RESIZE_REQUIRED		= false;
		static core::scolor S_WHITE			= { 255, 255, 255, 255 };
		static core::scolor S_BLACK			= { 0, 0, 0, 0 };
		static core::scolor S_BLANK			= { 0, 0, 0, 255 };

		//- defaults
		constexpr float C_DEFAULT_ROTATION	= 0.0f;
		constexpr vec2_t C_DEFAULT_SCALE	= { 1.0f, 1.0f };
		constexpr vec2_t C_DEFAULT_ORIGIN	= { 0.5f, 0.5f };
		static const auto C_DEFAULT_SOURCE	= core::srect{ 0.0f, 0.0f, 1.0f, 1.0f };

		//- Load some default assets such as default shaders, images and textures etc.
		//------------------------------------------------------------------------------------------------------------------------
		void load_internal_resources()
		{
			//- set default data for rendering
			auto& renderdata = ctx().m_render_data;
			renderdata.m_default_renderstate.m_blending = { blending_mode_alpha, blending_equation_blend_color, blending_factor_src_color, blending_factor_one_minus_src_color };
			renderdata.m_default_renderstate.m_flags = 0;

			ctx().m_render_data.m_default_shader = core::cservice_manager::find<cshader_manager>()->load_sync("sprite", shader_type_fragment,
				shaders::sprite::C_VS, shaders::sprite::C_PS);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void update_window_size(unsigned w, unsigned h)
		{
			ctx().m_os_data.m_window_w = w;
			ctx().m_os_data.m_window_h = h;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void update_window_viewport()
		{
			const auto& os_data = ctx().m_os_data;

			entry::get_renderer()->update_viewport({ os_data.m_window_x, os_data.m_window_y },
				{ os_data.m_window_w, os_data.m_window_h });
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

			auto& osdata = ctx().m_os_data;
			auto& renderdata = ctx().m_render_data;
			auto& layerdata = renderdata.m_layer_data;

			osdata.m_delta_time = entry::get_os()->frametime();

			//- platforms may need to handle events
			if (entry::get_os()->optional_process_event() == opresult_fail)
			{
				S_RUNNING = false;
			}

			//- check for resize of most basic layer. Other layers are not our responsibility
			if (S_RESIZE_REQUIRED)
			{
				layerdata.m_layers[0].m_target.resize(osdata.m_window_w, osdata.m_window_h);

				S_RESIZE_REQUIRED = false;
			}

			//- update HID state, such as keyboard and mouse
			entry::get_app()->on_update(osdata.m_delta_time);

			//- render frame
			entry::get_renderer()->update_viewport({ osdata.m_window_x, osdata.m_window_y }, { osdata.m_window_w, osdata.m_window_h });

			//- most basic layer, does always exist
			layerdata.m_layers[0].m_show = true;
			entry::get_renderer()->prepare_frame();
			entry::get_renderer()->blendmode(renderdata.m_default_renderstate.m_blending);

			//- layered rendering, from bottom to top
			for (auto i = 0u; i < layerdata.m_layer_count; ++i)
			{
				auto& layer = layerdata.m_layers[i];

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
			for (auto i = 0u; i < layerdata.m_layer_count; ++i)
			{
				auto& layer = layerdata.m_layers[i];

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
			update_window_size(ctx().m_os_data.m_window_w, ctx().m_os_data.m_window_h);
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
				cimage image;
				image.create_solid(256, 256, core::scolor(core::common_color_magenta));

				ctx().m_render_data.m_placeholder_texture = core::cservice_manager::find<ctexture_manager>()->load_sync("placeholder", image);

				cimage::destroy(image);
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
					ctx().m_os_data.m_window_w = e.w;
					ctx().m_os_data.m_window_h = e.h;
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
		entry::get_os()->main_window_position(&ctx().m_os_data.m_window_x, &ctx().m_os_data.m_window_y);
		entry::get_os()->main_window_size(&ctx().m_os_data.m_window_w, &ctx().m_os_data.m_window_h);
		S_FULLSCREEN = fullscreen;
		S_VSYNC = vsync;

		//- starting up graphics and app
		if (entry::get_os()->init_gfx(ctx().m_os_data.m_window_w, ctx().m_os_data.m_window_h, S_FULLSCREEN, S_VSYNC) != opresult_ok)
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
	sm::scontext& ctx()
	{
		static scontext S_CONTEXT;
		return S_CONTEXT;
	}

	//------------------------------------------------------------------------------------------------------------------------
	vec2_t window_size()
	{
		return vec2_t{ SCAST(float, raylib::GetScreenWidth()), SCAST(float, raylib::GetScreenHeight()) };
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned create_layer()
	{
		auto& renderdata = ctx().m_render_data;
		auto& layerdata = renderdata.m_layer_data;

		if (layerdata.m_layer_count < scontext::srender::slayers::C_LAYER_COUNT_MAX)
		{
			auto& layer = layerdata.m_layers[layerdata.m_layer_count];

			//- create with reasonable defaults
			layer.m_target.create(ctx().m_os_data.m_window_w, ctx().m_os_data.m_window_h);
			layer.m_combine_tint = S_WHITE;
			layer.m_clear_tint = S_WHITE;
			layer.m_flags = 0;
			layer.m_show = true;

			return ++layerdata.m_layer_count;
		}
		return MAX(unsigned);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::slayer& get_layer(unsigned layer)
	{
		auto& renderdata = ctx().m_render_data;
		auto& layerdata = renderdata.m_layer_data;

		CORE_ASSERT(layer < layerdata.m_layer_count, "Invalid operation. Accessed layer does not exist!");

		return layerdata.m_layers[layer];
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_line(unsigned layer, const vec2_t& start, const vec2_t& end, float thick, const core::scolor& color)
	{
		SM_DRAW_CALL(6);

		auto& renderdata = ctx().m_render_data;
		auto& layerdata = renderdata.m_layer_data;
		auto& command = layerdata.m_layers[layer].m_commands.emplace_back();

		command.create([=]()
			{
				raylib::DrawLineEx({ start.x, start.y }, { end.x, end.y }, thick, to_cliteral(color));
			});
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_circle(unsigned layer, const vec2_t& center, float radius, const core::scolor& color)
	{
		SM_DRAW_CALL(4);

		auto& renderdata = ctx().m_render_data;
		auto& layerdata = renderdata.m_layer_data;
		auto& command = layerdata.m_layers[layer].m_commands.emplace_back();

		command.create([=]()
			{
				raylib::DrawCircle(center.x, center.y, radius, to_cliteral(color));
			});
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_rect(unsigned layer, const vec2_t& position, const vec2_t& dimension, const core::scolor& color)
	{
		SM_DRAW_CALL(4);

		auto& renderdata = ctx().m_render_data;
		auto& layerdata = renderdata.m_layer_data;
		auto& command = layerdata.m_layers[layer].m_commands.emplace_back();

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
		draw_texture(layer, position, ctx().m_render_data.m_placeholder_texture, tint, C_DEFAULT_ROTATION, scale);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture)
	{
		draw_texture(layer, position, texture, S_WHITE);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture, const core::scolor& tint)
	{
		draw_texture(layer, position, texture, tint, C_DEFAULT_ROTATION);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture, const core::scolor& tint, float rotation)
	{
		draw_texture(layer, position, texture, tint, rotation, C_DEFAULT_SCALE);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture, const core::scolor& tint, float rotation,
		const vec2_t& scale)
	{
		draw_texture(layer, position, texture, tint, rotation, scale, ctx().m_render_data.m_default_shader);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture, const core::scolor& tint, float rotation,
		const vec2_t& scale, shader_handle_t shader)
	{
		draw_texture(layer, position, texture, tint, rotation, scale, shader, ctx().m_render_data.m_default_renderstate);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture, const core::scolor& tint, float rotation,
		const vec2_t& scale, shader_handle_t shader, const srenderstate& state)
	{
		draw_texture(layer, position, texture, tint, rotation, scale, shader, state, C_DEFAULT_ORIGIN);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture, const core::scolor& tint, float rotation,
		const vec2_t& scale, shader_handle_t shader, const srenderstate& state, const vec2_t& origin)
	{
		draw_texture(layer, position, texture, tint, rotation, scale, shader, state, origin, C_DEFAULT_SOURCE);
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

			ctx().m_render_data.m_layer_data.m_layers[layer].m_commands.emplace_back().create([=]()
				{
					auto& renderdata = ctx().m_render_data;
					auto& statedata = renderdata.m_state_data;

					//- TODO: think of a better way to get actual shaders, textures etc. from resource managers,
					//- Note: these command are executed on main thread, but other threads may still post load/unload
					//- requests, so we have to account for that and make them thread-safe.
					const auto& tm = *core::cservice_manager::find<ctexture_manager>();
					const auto& sm = *core::cservice_manager::find<cshader_manager>();

					//- texture and dimension
					const auto& _texture = tm.at(texture == C_INVALID_HANDLE ? renderdata.m_placeholder_texture : texture);
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
					if (shader != statedata.m_shader)
					{
						statedata.m_shader = shader;
					}

					//- setting shader is always enabled as we set default shader manually
					const auto& _shader = sm.at(statedata.m_shader);
					raylib::BeginShaderMode(_shader.shader());

					//- check if renderstate has changed since last draw command
					if (state != statedata.m_renderstate)
					{
						statedata.m_renderstate = state;
					}

					//- set blending mode only if enabled for this draw command
					if (algorithm::bit_check(state.m_flags, renderable_flag_blending_custom))
					{
						auto& renderstate = statedata.m_renderstate;

						raylib::rlSetBlendMode(renderstate.m_blending.m_mode);
						raylib::rlSetBlendFactors(renderstate.m_blending.m_src_factor, renderstate.m_blending.m_dst_factor, renderstate.m_blending.m_equation);
						raylib::rlSetBlendMode(raylib::BLEND_CUSTOM);
					}

					raylib::DrawTexturePro(_texture.texture(), src, dst, orig, rotation, tint.cliteral<raylib::Color>());
				});
		}
	}

} //- sm