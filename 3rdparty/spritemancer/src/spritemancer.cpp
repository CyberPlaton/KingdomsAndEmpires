#include "spritemancer.hpp"
#include "detail/os/sm_os_headless.hpp"
#include "detail/renderers/sm_renderer_headless.hpp"
#include "detail/renderers/sm_renderer_bgfx.hpp"
#include "detail/os/sm_os_glfw.hpp"
#include "detail/sm_embedded_shaders.hpp"
#include "detail/sm_context.hpp"
#include "detail/imgui_integration/imgui.hpp"
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

		//- FIXME: this will create ALL renderpasses that exist, without considering if they are enabled or not.
		//------------------------------------------------------------------------------------------------------------------------
		void engine_setup_renderpasses()
		{
			log_debug("Create gfx renderpasses:");

			for (const auto& type : rttr::type::get<renderpass::irenderpass>().get_derived_classes())
			{
				const auto& pass = renderpass_create(type);

				log_debug(fmt::format("\tType: '{}', Id: '{}'\n\tConfig: '{}'\n",
					type.get_name().data(),
					pass->m_cfg.m_id,
					core::io::to_json_string(pass->m_cfg, true)));
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		void engine_sync_context_data()
		{
			auto& os = entry::ctx()->io();

			//- Window dimensions and position
			{
				int x = 0, y = 0, w = 0, h = 0;

				entry::os()->main_window_position(&x, &y);
				entry::os()->main_window_size(&w, &h);

				if (os.m_window_w != w || os.m_window_h != h)
				{
					os.m_window_w = w;
					os.m_window_h = h;
					entry::ctx()->want_resize(true);
				}
				os.m_window_x = x;
				os.m_window_y = y;
			}

			os.m_dt = entry::os()->frametime();
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
					entry::ctx()->running(false);
				}
				engine_sync_context_data();
			}

			//- check for resize of most basic layer. Other layers are not our responsibility
			if (entry::ctx()->want_resize())
			{
				const auto& io = entry::ctx()->io();
				entry::renderer()->reset(io.m_window_w, io.m_window_h, io.m_fullscreen, io.m_vsync);
				imgui::cui::on_resize(io.m_window_w, io.m_window_h);
				entry::ctx()->want_resize(false);
			}

			//- update HID state, such as keyboard and mouse
			{
				CORE_NAMED_ZONE(app_on_update);
				entry::app()->on_update(entry::ctx()->io().m_dt);
			}

			//- render frame
			{
				CORE_NAMED_ZONE(renderer_prepare_frame);
				entry::renderer()->begin();
			}

			//- imgui drawing
			{
				CORE_NAMED_ZONE(app_imgui);
				imgui::begin();
				entry::app()->on_imgui();
				imgui::end();
			}

			//- present everything
			{
				CORE_NAMED_ZONE(renderer_frame_present);
				//- TODO: finalize rendering with imgui on top of everything
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

			auto& os = entry::ctx()->io();

			entry::os()->main_window_position(&os.m_window_x, &os.m_window_y);
			entry::os()->main_window_size(&os.m_window_w, &os.m_window_h);
			os.m_fullscreen = fullscreen;
			os.m_vsync = vsync;

			if (entry::os()->init_gfx(os.m_window_w, os.m_window_h, fullscreen, vsync) != opresult_ok)
			{
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(core::logging_verbosity_error,
						"Failed initializing GFX!");
				}
				return;
			}

			//- create resource managers and load default data
			core::cservice_manager::emplace<cimage_manager>();
			core::cservice_manager::emplace<ctexture_manager>();
			core::cservice_manager::emplace<cshader_manager>();
			core::cservice_manager::emplace<cspriteatlas_manager>();
			core::cservice_manager::emplace<crendertarget_manager>();
			core::cservice_manager::emplace<cmesh_manager>();
			core::cservice_manager::emplace<cmaterial_manager>();
			core::cservice_manager::emplace<cprogram_manager>();
			core::cservice_manager::emplace<cuniform_manager>();

			//- initialize client application
			if (!entry::app()->on_init(entry::ctx()->user_data()))
			{
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(core::logging_verbosity_error,
						"Failed initializing client application!");
				}
				return;
			}

			if (entry::os()->post_init() != opresult_ok)
			{
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(core::logging_verbosity_error,
						"Failed post initializing OS!");
				}
				return;
			}

			//- create imgui context
			imgui::init();
			imgui::cui::on_resize(os.m_window_w, os.m_window_h);

			//- finalize init with creation of renderpasses
			//- TODO: we want this not to be on init, but rather dynamically changeable and configurable, we might setup only the main renderpass here
			engine_setup_renderpasses();

			//- main loop
			while (entry::ctx()->running())
			{
				engine_update();
			}

			//- shutting down
			entry::app()->on_shutdown();
			imgui::shutdown();
			entry::os()->shutdown();
			if (entry::has_platform()) { entry::platform()->shutdown(); }
			entry::destroy_context();
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult prepare(iapp* app, void* config)
	{
		if (!entry::create_context())
		{
			return opresult_fail;
		}

		entry::ctx()->user_data(config);

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

		entry::ctx()->running(true);

		//- start the thread with window and graphics context and proceed init there
		std::thread thread(engine_thread, title, w, h, fullscreen, vsync);

		thread.join();

		//- end of the thread and cleanup without requiring window and graphics context
		if (entry::has_platform()) { entry::platform()->post_shutdown(); }

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ccontext& ctx()
	{
		static ccontext S_CONTEXT; return S_CONTEXT;
	}

	//------------------------------------------------------------------------------------------------------------------------
	vec2_t window_size()
	{
		auto& os = entry::ctx()->io();

		return { SCAST(float, os.m_window_w), SCAST(float, os.m_window_h) };
	}

	//------------------------------------------------------------------------------------------------------------------------
	vec2_t window_position()
	{
		auto& os = entry::ctx()->io();

		return { SCAST(float, os.m_window_x), SCAST(float, os.m_window_y) };
	}

	//------------------------------------------------------------------------------------------------------------------------
	const sm::renderpasses_t& renderpasses()
	{
		return entry::ctx()->graphics().m_renderpasses;
	}

	//------------------------------------------------------------------------------------------------------------------------
	renderpass_ref_t renderpass_create(stringview_t type)
	{
		return renderpass_create(rttr::type::get_by_name(type.data()));
	}

	//------------------------------------------------------------------------------------------------------------------------
	renderpass_ref_t renderpass_create(rttr::type type)
	{
		auto& gfx = entry::ctx()->graphics();

		auto& pass = gfx.m_renderpasses.emplace_back(rttr::detail::invoke_static_function(type, renderpass::irenderpass::C_RENDERPASS_CREATE_FUNC_NAME)
			.get_value<renderpass_ref_t>());

		gfx.m_renderpass_order[pass->m_cfg.m_id] = pass;

		return pass;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void renderpass_begin(const renderpass_ref_t& pass)
	{
		entry::renderer()->renderpass_begin(pass);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void renderpass_end(const renderpass_ref_t& pass)
	{
		entry::renderer()->renderpass_end(pass);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void renderpass_reset(const renderpasses_t& passes)
	{
		entry::renderer()->renderpass_reset(passes);
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
    void draw_mesh(const void* mtx, mesh_handle_t mesh_handle, material_handle_t material_handle)
    {
        auto* meshm = core::cservice_manager::find<cmesh_manager>();
        auto* matm = core::cservice_manager::find<cmaterial_manager>();

        auto& mesh = meshm->get(mesh_handle);
        auto& material = matm->get(material_handle);

		bgfx::setTransform(mtx);

        mesh.bind();
        material.bind();
    }

} //- sm

RTTR_REGISTRATION
{
	using namespace sm;

//------------------------------------------------------------------------------------------------------------------------
rttr::cregistrator<cspriteatlas>("cspriteatlas")
	.meta(core::cresource::C_META_SUPPORTED_EXTENSIONS,
		vector_t<string_t>{".png", ".bmp", ".tga", ".jpg", ".gif", ".pic",
		".psd", ".hdr", ".qoi", ".svg", ".dds", ".pkm", ".ktx", ".pvr", ".astc"})
	;

//------------------------------------------------------------------------------------------------------------------------
rttr::cregistrator<crendertarget>("crendertarget")
	.meta(core::cresource::C_META_SUPPORTED_EXTENSIONS, vector_t<string_t>{})
;

//------------------------------------------------------------------------------------------------------------------------
rttr::cregistrator<ctexture>("ctexture")
	.meta(core::cresource::C_META_SUPPORTED_EXTENSIONS,
		vector_t<string_t>{".png", ".bmp", ".tga", ".jpg", ".gif", ".pic",
		".psd", ".hdr", ".qoi", ".svg", ".dds", ".pkm", ".ktx", ".pvr", ".astc"})
	;

//------------------------------------------------------------------------------------------------------------------------
rttr::cregistrator<cimage>("cimage")
	.meta(core::cresource::C_META_SUPPORTED_EXTENSIONS,
		vector_t<string_t>{".png", ".bmp", ".tga", ".jpg", ".gif", ".pic",
		".psd", ".hdr", ".qoi", ".svg", ".dds", ".pkm", ".ktx", ".pvr", ".astc"})
	;

//------------------------------------------------------------------------------------------------------------------------
rttr::cregistrator<cshader>("cshader")
	.meta(core::cresource::C_META_SUPPORTED_EXTENSIONS, vector_t<string_t>{".vs", ".fs"})
	;

//------------------------------------------------------------------------------------------------------------------------
rttr::cregistrator<cprogram>("cprogram")
	.meta(core::cresource::C_META_SUPPORTED_EXTENSIONS, vector_t<string_t>{".vs", ".fs"})
	;

//------------------------------------------------------------------------------------------------------------------------
rttr::cregistrator<cuniform>("cuniform")
	.meta(core::cresource::C_META_SUPPORTED_EXTENSIONS, vector_t<string_t>{})
	;

//------------------------------------------------------------------------------------------------------------------------
rttr::cregistrator<cmesh>("cmesh")
	.meta(core::cresource::C_META_SUPPORTED_EXTENSIONS, vector_t<string_t>{})
	;

//------------------------------------------------------------------------------------------------------------------------
rttr::cregistrator<cmaterial>("cmaterial")
	.meta(core::cresource::C_META_SUPPORTED_EXTENSIONS, vector_t<string_t>{})
	;

using namespace sm::renderpass;

//------------------------------------------------------------------------------------------------------------------------
rttr::registration::enumeration<view_mode>("view_mode")
(
	rttr::value("view_mode_none", view_mode_none),
	rttr::value("view_mode_default", view_mode_default),
	rttr::value("view_mode_sequential", view_mode_sequential),
	rttr::value("view_mode_depth_ascending", view_mode_depth_ascending),
	rttr::value("view_mode_depth_descending", view_mode_depth_descending)
);

//------------------------------------------------------------------------------------------------------------------------
rttr::registration::enumeration<framebuffer_ratio>("framebuffer_ratio")
(
	rttr::value("framebuffer_ratio_none", framebuffer_ratio_none),
	rttr::value("framebuffer_ratio_equal", framebuffer_ratio_equal),
	rttr::value("framebuffer_ratio_half", framebuffer_ratio_half),
	rttr::value("framebuffer_ratio_quarter", framebuffer_ratio_quarter),
	rttr::value("framebuffer_ratio_eighth", framebuffer_ratio_eighth),
	rttr::value("framebuffer_ratio_sixteenth", framebuffer_ratio_sixteenth),
	rttr::value("framebuffer_ratio_double", framebuffer_ratio_double)
);

//------------------------------------------------------------------------------------------------------------------------
rttr::registration::enumeration<uniform_type>("uniform_type")
(
	rttr::value("uniform_type_none", uniform_type_none),
	rttr::value("uniform_type_float", uniform_type_float),
	rttr::value("uniform_type_vector2", uniform_type_vector2),
	rttr::value("uniform_type_vector3", uniform_type_vector3),
	rttr::value("uniform_type_vector4", uniform_type_vector4),
	rttr::value("uniform_type_mat3x3", uniform_type_mat3x3),
	rttr::value("uniform_type_mat4x4", uniform_type_mat4x4),
	rttr::value("uniform_type_sampler", uniform_type_sampler)
);

//------------------------------------------------------------------------------------------------------------------------
rttr::cregistrator<sconfig>("sconfig")
	.prop("m_id", &sconfig::m_id)
	.prop("m_view_mode", &sconfig::m_view_mode)
	.prop("m_rendertarget_ratio", &sconfig::m_rendertarget_ratio)
	.prop("m_rendertarget", &sconfig::m_rendertarget)
	.prop("m_program", &sconfig::m_program)
	.prop("m_flags", &sconfig::m_flags)
	;

//------------------------------------------------------------------------------------------------------------------------
rttr::cregistrator<irenderpass>("irenderpass")
	.meth(irenderpass::C_RENDERPASS_NAME_FUNC_NAME, &irenderpass::name)
	.meth(irenderpass::C_RENDERPASS_CONFIG_FUNC_NAME, &irenderpass::config)
	.meth(irenderpass::C_RENDERPASS_CREATE_FUNC_NAME, &irenderpass::create)
	.prop("m_cfg", &irenderpass::m_cfg)
	;

//------------------------------------------------------------------------------------------------------------------------
rttr::cregistrator<smain_renderpass>("smain_renderpass")
	.meth(irenderpass::C_RENDERPASS_NAME_FUNC_NAME, &smain_renderpass::name)
	.meth(irenderpass::C_RENDERPASS_CONFIG_FUNC_NAME, &smain_renderpass::config)
	.meth(irenderpass::C_RENDERPASS_CREATE_FUNC_NAME, &smain_renderpass::create)
	;
}