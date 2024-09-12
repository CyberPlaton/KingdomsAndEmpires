#include "spritemancer.hpp"
#include "detail/os/sm_os_headless.hpp"
#include "detail/renderers/sm_renderer_headless.hpp"
#include "detail/renderers/sm_renderer_bgfx.hpp"
#include "detail/os/sm_os_glfw.hpp"
#include "detail/sm_resource_manager.hpp"
#include "detail/sm_embedded_shaders.hpp"

namespace sm
{
	namespace
	{
		static core::cmutex S_MUTEX;
		static queue_t<rttr::variant> S_EVENT_QUEUE;

		static program_handle_t S_DEFAULT_PROGRAM = 0;
		static core::scolor S_WHITE = { 255, 255, 255, 255 };
		static core::scolor S_BLACK = { 0, 0, 0, 0 };
		static core::scolor S_BLANK = { 0, 0, 0, 255 };
		static std::atomic_bool S_RUNNING;
		static bool S_RESIZE_REQUIRED = false;
		static float S_DT = 0.0f;
		static texture_handle_t S_PLACEHOLDER_TEXTURE = 0;
		static blending_mode S_BLEND_MODE_DEFAULT = { blending_mode_alpha };
		static srenderstate S_RENDERSTATE_DEFAULT = { S_BLEND_MODE_DEFAULT, 0 };
		constexpr float C_ROTATION_DEFAULT = 0.0f;
		constexpr vec2_t C_SCALE_DEFAULT = {1.0f, 1.0f};
		constexpr vec2_t C_ORIGIN_DEFAULT = {0.5f, 0.5f};
		static inline const core::srect C_SOURCE_DEFAULT = {0.0f, 0.0f, 1.0f, 1.0f};

		//- current used rendering state data
		static blending_mode S_CURRENT_BLEND_MODE = S_BLEND_MODE_DEFAULT;
		static srenderstate S_CURRENT_RENDERSTATE = S_RENDERSTATE_DEFAULT;
		static program_handle_t S_CURRENT_PROGRAM = S_DEFAULT_PROGRAM;

		static void* S_CONFIG = nullptr;
		static argparse::ArgumentParser S_ARGS;

		//- TODO: get all that stuff from above and move to here
		//------------------------------------------------------------------------------------------------------------------------
		struct scontext
		{

			//- Window related data
			core::srect m_window_rect =  {0.0f, 0.0f, 0.0f, 0.0f};
			int m_window_flags = 0;
			void* m_window_handle = nullptr;
		};


		//- Load some default assets such as default shaders, images and textures etc.
		//------------------------------------------------------------------------------------------------------------------------
		void load_internal_resources()
		{
			S_DEFAULT_PROGRAM = core::cservice_manager::find<cprogram_manager>()->load_sync("sprite", {bgfx::kInvalidHandle}, { bgfx::kInvalidHandle });
		}

		//------------------------------------------------------------------------------------------------------------------------
		void update_os_events()
		{
			while (!S_EVENT_QUEUE.empty())
			{
				const auto& event = S_EVENT_QUEUE.front();

				entry::get_os()->on_event(event);
				entry::get_renderer()->on_event(event);

				S_EVENT_QUEUE.pop();
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		void engine_configure_platform_and_renderer(iapp* app)
		{
			entry::set_app(app);
			entry::set_os(std::make_unique<cos_glfw>());
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
			if (entry::get_os()->optional_process_event() == opresult_fail)
			{
				S_RUNNING = false;
			}

			update_os_events();

			//- tick one frame 
			entry::get_renderer()->frame_begin();

			entry::get_app()->on_update(S_DT);

			entry::get_app()->on_imgui();

			entry::get_renderer()->frame_end();
		}

		//------------------------------------------------------------------------------------------------------------------------
		void engine_prepare(const int w, const int h)
		{
			entry::get_renderer()->update_viewport({0.0f, 0.0f}, {w, h});

			//- create resource managers
			core::cservice_manager::emplace<cimage_manager>();
			core::cservice_manager::emplace<ctexture_manager>();
			core::cservice_manager::emplace<cshader_manager>();
			core::cservice_manager::emplace<cprogram_manager>();
			core::cservice_manager::emplace<cspriteatlas_manager>();
			core::cservice_manager::emplace<crendertarget_manager>();

			load_internal_resources();

			//- create default placeholder texture
			{
				cimage solid; solid.create_solid(1, 1, S_WHITE);

				S_PLACEHOLDER_TEXTURE = core::cservice_manager::find<ctexture_manager>()->load_sync("placeholder", solid);

				cimage::destroy(solid);
			}

			//- create default font

			//- create default rendering layer
			//create_layer();

			//- sample time for custom frame timing
		}

		//------------------------------------------------------------------------------------------------------------------------
		void engine_finalize_init()
		{
			//- initialize imgui
			imgui::init();

			//- create internal event listeners to update our own state and redirect data to other
			//- dependent systems such as ios.
			auto* es = core::cservice_manager::find<core::cevent_service>();
			es->emplace_listener<events::window::sresize>([](const rttr::variant& var){ S_EVENT_QUEUE.push(var); });
			es->emplace_listener<events::window::scursor>([](const rttr::variant& var){ S_EVENT_QUEUE.push(var); });
			es->emplace_listener<events::window::smouse_button>([](const rttr::variant& var) { S_EVENT_QUEUE.push(var); });
			es->emplace_listener<events::window::skey_button>([](const rttr::variant& var) { S_EVENT_QUEUE.push(var); });
			es->emplace_listener<events::window::sminimize>([](const rttr::variant& var) { S_EVENT_QUEUE.push(var); });
			es->emplace_listener<events::window::sunminimize>([](const rttr::variant& var) { S_EVENT_QUEUE.push(var); });
			es->emplace_listener<events::window::shide>([](const rttr::variant& var) { S_EVENT_QUEUE.push(var); });
			es->emplace_listener<events::window::sunhide>([](const rttr::variant& var) { S_EVENT_QUEUE.push(var); });
			es->emplace_listener<events::window::sfocus>([](const rttr::variant& var) { S_EVENT_QUEUE.push(var); });
			es->emplace_listener<events::window::sunfocus>([](const rttr::variant& var) { S_EVENT_QUEUE.push(var); });
			es->emplace_listener<events::window::smouse_scroll>([](const rttr::variant& var) { S_EVENT_QUEUE.push(var); });
			es->emplace_listener<events::window::scharacter_input>([](const rttr::variant& var) { S_EVENT_QUEUE.push(var); });
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

		//- starting up graphics and app
		if (entry::get_os()->init_gfx(w, h, fullscreen, vsync) != opresult_ok)
		{
			return opresult_fail;
		}

		engine_prepare(w, h);

		entry::get_app()->on_init(S_CONFIG, S_ARGS);

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
	vec2_t window_size()
	{
		int w = 0, h = 0;

		entry::get_os()->main_window_size(&w, &h);

		return vec2_t{ SCAST(float, w), SCAST(float, h) };
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_primitives(unsigned layer, const cvertices& vertices, const indices_t& indices, program_handle_t program,
		const core::scolor& color, srenderstate state)
	{
		bgfx::TransientVertexBuffer tvb;
		bgfx::allocTransientVertexBuffer(&tvb, vertices.count(), vertices.declaration());

		bx::memCopy(&tvb, vertices.data(), vertices.size());

		bgfx::TransientIndexBuffer tib;
		bgfx::allocTransientIndexBuffer(&tib, SCAST(unsigned, indices.size()));

		bgfx::setVertexBuffer(0, &tvb);
		bgfx::setIndexBuffer(&tib);

		//- TODO: set uniforms for textures and textures

		bgfx::setState(state.m_blending | state.m_bgfx_state, color.abgr());
		bgfx::submit(bgfx::ViewId{ SCAST(uint16_t, layer) }, bgfx::ProgramHandle{ SCAST(uint16_t, program) });
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_triangles(unsigned layer, const cvertices& vertices, const indices_t& indices, program_handle_t program,
		const core::scolor& color, srenderstate state)
	{
		algorithm::bit_clear(state.m_bgfx_state, BGFX_STATE_PT_MASK);

		draw_primitives(layer, vertices, indices, program, color, state);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void set_logger(core::error_report_function_t callback)
	{
		serror_reporter::instance().m_callback = std::move(callback);
	}

} //- sm