#include "spritemancer.hpp"
#include "detail/os/sm_os_headless.hpp"
#include "detail/renderers/sm_renderer_headless.hpp"
#include "detail/renderers/sm_renderer_bgfx.hpp"
#include "detail/os/sm_os_glfw.hpp"
#include "detail/sm_resource_manager.hpp"
#include "detail/sm_embedded_shaders.hpp"
#if CORE_PLATFORM_WINDOWS && PROFILE_ENABLE && TRACY_ENABLE
#include <tracy.h>
#endif

namespace sm
{
	namespace
	{
		//------------------------------------------------------------------------------------------------------------------------
		void engine_configure_platform_and_renderer(iapp* app)
		{
			entry::set_app(app);
			entry::set_os(std::make_unique<cos_glfw>());
			entry::set_renderer(std::make_unique<crenderer_bgfx>());
		}

		//------------------------------------------------------------------------------------------------------------------------
		void engine_update()
		{
			CORE_ZONE;

			entry::os()->frametime();

			//- platforms may need to handle events
			{
				CORE_NAMED_ZONE(os_process_events);
				if (entry::os()->process_events() == opresult_fail)
				{
					ctx()->running(false);
				}
			}

			//- check for resize of most basic layer. Other layers are not our responsibility
			if (ctx()->want_resize())
			{
				const auto& io =  ctx()->io();

				entry::renderer()->update_viewport({ io.m_window_x, io.m_window_y },
					{ io.m_window_w, io.m_window_h });

				ctx()->want_resize(false);
			}

			//- update HID state, such as keyboard and mouse
			{
				CORE_NAMED_ZONE(app_on_update);
				entry::app()->on_update(osdata.m_delta_time);
			}

			//- render frame
			//- most basic layer, does always exist
			{
				CORE_NAMED_ZONE(renderer_prepare_frame);
				layerdata.m_rendering_layers[0].m_show = true;
				entry::renderer()->update_frame_camera(renderdata.m_frame_camera);
				entry::renderer()->begin();
				entry::renderer()->clear();
				entry::renderer()->blendmode(renderdata.m_default_renderstate.m_blending);
			}

			//- layered rendering, from bottom to top
			{
				CORE_NAMED_ZONE(renderer_layered_rendering);
				for (auto i = 0u; i < layerdata.m_layer_count; ++i)
				{
					auto& layer = layerdata.m_rendering_layers[i];

					{
						CORE_NAMED_ZONE(renderer_layer_draw);
						entry::renderer()->layer_draw(layer);
					}

					layer.m_commands.clear();
				}

				//- layered debug rendering if enabled
				if (layerdata.m_layer_debug_enabled)
				{
					entry::renderer()->layer_draw(get_layer_debug());
				}
			}

			//- present everything
			{
				CORE_NAMED_ZONE(renderer_frame_present);
				//- finalize rendering with imgui on top of everything
				{
					CORE_NAMED_ZONE(renderer_imgui_draw);
					if (entry::renderer()->imgui_begin())
					{
						entry::renderer()->state_reset_to_default();
						entry::app()->on_imgui();
						entry::renderer()->imgui_end();
					}
				}

				entry::renderer()->end();
			}

#if CORE_PLATFORM_WINDOWS && PROFILE_ENABLE && TRACY_ENABLE
			FrameMark;
#endif
		}

		//- Main engine thread where the update and rendering happens. Created from outside
		//------------------------------------------------------------------------------------------------------------------------
		void engine_thread(stringview_t title, unsigned w, unsigned h, bool fullscreen, bool vsync)
		{
			if (entry::os()->init_mainwindow(title.data(), w, h, fullscreen) != opresult_ok)
			{
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(core::logging_verbosity_error,
						"Failed initializing OS mainwindow!");
				}
				return;
			}

			if (entry::has_platform() && entry::platform()->init() != opresult_ok)
			{
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(core::logging_verbosity_error,
						"Failed initializing platform!");
				}
				return;
			}

			entry::os()->main_window_position(&ctx().m_os_data.m_window_x, &ctx().m_os_data.m_window_y);
			entry::os()->main_window_size(&ctx().m_os_data.m_window_w, &ctx().m_os_data.m_window_h);
			ctx().m_os_data.m_fullscreen = fullscreen;
			ctx().m_os_data.m_vsync = vsync;

			if (entry::os()->init_gfx(ctx().m_os_data.m_window_w, ctx().m_os_data.m_window_h,
				fullscreen, vsync) != opresult_ok)
			{
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(core::logging_verbosity_error,
						"Failed initializing GFX!");
				}
				return;
			}

			entry::renderer()->update_viewport({ ctx().m_os_data.m_window_x, ctx().m_os_data.m_window_y },
				{ ctx().m_os_data.m_window_w, ctx().m_os_data.m_window_h });

			//- create resource managers and load default data
			core::cservice_manager::emplace<cimage_manager>();
			core::cservice_manager::emplace<ctexture_manager>();
			core::cservice_manager::emplace<cshader_manager>();
			core::cservice_manager::emplace<cspriteatlas_manager>();
			core::cservice_manager::emplace<crendertarget_manager>();

			auto& renderdata = ctx().m_render_data;
			renderdata.m_default_renderstate.m_flags = 0;

			ctx().m_render_data.m_default_shader = core::cservice_manager::find<cshader_manager>()->load_sync("sprite",
				shader_type_fragment, shaders::sprite::C_VS, shaders::sprite::C_PS);

			{
				cimage image;
				image.create_solid(256, 256, core::scolor(core::common_color_magenta));

				ctx().m_render_data.m_placeholder_texture = core::cservice_manager::find<ctexture_manager>()->load_sync("placeholder", image);

				cimage::destroy(image);
			}

			//- finalize preparation
			create_layer();

			//- initialize client application
			if (!entry::app()->on_init(S_CONFIG))
			{
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(core::logging_verbosity_error,
						"Failed initializing client application!");
				}
				return;
			}

			core::cservice_manager::find<core::cevent_service>()->emplace_listener<events::window::sresize>([](const rttr::variant& var)
				{
					const auto& e = var.convert<events::window::sresize>();
					ctx().m_os_data.m_window_w = e.w;
					ctx().m_os_data.m_window_h = e.h;
					S_RESIZE_REQUIRED = true;
				});

			//- create imgui context
			imgui::init();

			//- main loop
			while (S_RUNNING)
			{
				engine_update();
			}

			//- shutting down
			entry::app()->on_shutdown();
			imgui::shutdown();
			entry::os()->shutdown();
			if (entry::has_platform()) { entry::platform()->shutdown(); }
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult prepare(iapp* app, void* config)
	{
		ctx()->user_data(config);

		engine_configure_platform_and_renderer(app);

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult run(stringview_t title, unsigned w, unsigned h, bool fullscreen, bool vsync)
	{
		//- setup and initialize things that do not require a window and graphics context
		if (entry::has_platform() && entry::platform()->pre_init() != opresult_ok)
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Failed pre-initializing platform!");
			}
			return opresult_fail;
		}
		if (entry::os()->init() != opresult_ok)
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Failed initializing OS!");
			}
			return opresult_fail;
		}

		S_RUNNING.store(true);

		//- start the thread with window and graphics context and proceed init there
		std::thread thread(engine_thread, title, w, h, fullscreen, vsync);

		thread.join();

		//- end of the thread and cleanup without requiring window and graphics context
		if (entry::has_platform()) { entry::platform()->post_shutdown(); }

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
	vec2_t window_position()
	{
		const auto p = raylib::GetWindowPosition();
		return vec2_t{ SCAST(float, p.x), SCAST(float, p.y) };
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned create_layer()
	{
		auto& renderdata = ctx().m_render_data;
		auto& layerdata = renderdata.m_layer_data;

		if (layerdata.m_layer_count < scontext::srender::srendering_layers::C_LAYER_COUNT_MAX)
		{
			auto& layer = layerdata.m_rendering_layers[layerdata.m_layer_count];

			//- create with reasonable defaults
			layer.m_id = layerdata.m_layer_count;
			layer.m_flags = 0;
			layer.m_show = true;

			return layerdata.m_layer_count++;
		}
		return MAX(unsigned);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void update_frame_camera(const vec2_t& position, const vec2_t& offset, float zoom, float rotation)
	{
		auto& renderdata = ctx().m_render_data;
		renderdata.m_frame_camera.m_position = position;
		renderdata.m_frame_camera.m_offset = position;
		renderdata.m_frame_camera.m_zoom = zoom;
		renderdata.m_frame_camera.m_rotation = rotation;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_line(unsigned layer, const vec2_t& start, const vec2_t& end, float thick, const core::scolor& color)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_circle(unsigned layer, const vec2_t& center, float radius, const core::scolor& color)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_rect(unsigned layer, const vec2_t& position, const vec2_t& dimension, const core::scolor& color)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void set_logger(core::error_report_function_t callback)
	{
		serror_reporter::instance().m_callback = std::move(callback);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void configure_args(const args_ref_t& args)
	{
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
		if (algorithm::bit_check(state.m_flags, renderable_flag_invisible))
		{
			return;
		}
	}

} //- sm

RTTR_REGISTRATION
{
	using namespace sm;

//------------------------------------------------------------------------------------------------------------------------
rttr::cregistrator<cspriteatlas>("cspriteatlas")
	.meth(core::cresource::C_DESTROY_FUNCTION_NAME.data(), &cspriteatlas::destroy)
	.meta(core::cresource::C_META_SUPPORTED_EXTENSIONS,
		vector_t<string_t>{".png", ".bmp", ".tga", ".jpg", ".gif", ".pic",
		".psd", ".hdr", ".qoi", ".svg", ".dds", ".pkm", ".ktx", ".pvr", ".astc"})
	;

//------------------------------------------------------------------------------------------------------------------------
rttr::cregistrator<crendertarget>("crendertarget")
	.meth(core::cresource::C_DESTROY_FUNCTION_NAME.data(), &crendertarget::destroy)
	.meta(core::cresource::C_META_SUPPORTED_EXTENSIONS, vector_t<string_t>{})
;

//------------------------------------------------------------------------------------------------------------------------
rttr::cregistrator<ctexture>("ctexture")
	.meth(core::cresource::C_DESTROY_FUNCTION_NAME.data(), &ctexture::destroy)
	.meta(core::cresource::C_META_SUPPORTED_EXTENSIONS,
		vector_t<string_t>{".png", ".bmp", ".tga", ".jpg", ".gif", ".pic",
		".psd", ".hdr", ".qoi", ".svg", ".dds", ".pkm", ".ktx", ".pvr", ".astc"})
	;

//------------------------------------------------------------------------------------------------------------------------
rttr::cregistrator<cimage>("cimage")
	.meth(core::cresource::C_DESTROY_FUNCTION_NAME.data(), &cimage::destroy)
	.meta(core::cresource::C_META_SUPPORTED_EXTENSIONS,
		vector_t<string_t>{".png", ".bmp", ".tga", ".jpg", ".gif", ".pic",
		".psd", ".hdr", ".qoi", ".svg", ".dds", ".pkm", ".ktx", ".pvr", ".astc"})
	;

//------------------------------------------------------------------------------------------------------------------------
rttr::cregistrator<cshader>("cshader")
	.meth(core::cresource::C_DESTROY_FUNCTION_NAME.data(), &cshader::destroy)
	.meta(core::cresource::C_META_SUPPORTED_EXTENSIONS, vector_t<string_t>{".vs", ".fs"})
	;

//------------------------------------------------------------------------------------------------------------------------
rttr::cregistrator<cprogram>("cprogram")
	.meth(core::cresource::C_DESTROY_FUNCTION_NAME.data(), &cprogram::destroy)
	.meta(core::cresource::C_META_SUPPORTED_EXTENSIONS, vector_t<string_t>{".vs", ".fs"})
	;
}