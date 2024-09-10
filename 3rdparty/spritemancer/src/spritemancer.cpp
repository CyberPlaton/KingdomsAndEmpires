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
		constexpr unsigned C_LAYER_COUNT_MAX = 256;
		static unsigned S_LAYER_COUNT = 0;
		static array_t<slayer, C_LAYER_COUNT_MAX> S_LAYERS;
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

		//- 
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

			entry::get_app()->on_update(S_DT);

			//- most basic layer, does always exist
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
	unsigned create_layer()
	{
		if (S_LAYER_COUNT < C_LAYER_COUNT_MAX)
		{
			auto& layer = S_LAYERS[S_LAYER_COUNT];
			int w, h;
			entry::get_os()->main_window_size(&w, &h);

			//- create with reasonable defaults
			layer.m_target.create(w, h);
			layer.m_combine_tint = S_WHITE;
			layer.m_clear_tint = S_WHITE;
			layer.m_flags = 0;
			layer.m_show = true;

			return S_LAYER_COUNT++;
		}
		return MAX(unsigned);
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
	void draw_line(unsigned layer, const vec2_t& start, const vec2_t& end, float thick, const core::scolor& color)
	{
		SM_DRAW_CALL(6); //- 6 vertices? Really???

		auto& command = S_LAYERS[layer].m_commands.emplace_back();

		command.create([=]()
			{
				/*raylib::DrawLineEx({ start.x, start.y }, { end.x, end.y }, thick, to_cliteral(color));*/
			});
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_circle(unsigned layer, const vec2_t& center, float radius, const core::scolor& color)
	{
		SM_DRAW_CALL(4);

		auto& command = S_LAYERS[layer].m_commands.emplace_back();

		command.create([=]()
			{
				/*raylib::DrawCircle(center.x, center.y, radius, to_cliteral(color));*/
			});
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_rect(unsigned layer, const vec2_t& position, const vec2_t& dimension, const core::scolor& color)
	{
		SM_DRAW_CALL(4);

		auto& command = S_LAYERS[layer].m_commands.emplace_back();

		command.create([=]()
			{
				/*raylib::DrawRectangleV({ position.x, position.y }, { dimension.x, dimension.y }, to_cliteral(color));*/
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
		draw_texture(layer, position, texture, tint, rotation, scale, S_DEFAULT_PROGRAM);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture, const core::scolor& tint, float rotation,
		const vec2_t& scale, program_handle_t program)
	{
		draw_texture(layer, position, texture, tint, rotation, scale, program, S_RENDERSTATE_DEFAULT);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture, const core::scolor& tint, float rotation,
		const vec2_t& scale, program_handle_t program, const srenderstate& state)
	{
		draw_texture(layer, position, texture, tint, rotation, scale, program, state, C_ORIGIN_DEFAULT);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture, const core::scolor& tint, float rotation,
		const vec2_t& scale, program_handle_t program, const srenderstate& state, const vec2_t& origin)
	{
		draw_texture(layer, position, texture, tint, rotation, scale, program, state, origin, C_SOURCE_DEFAULT);
	}

	//- The actual rendering routine for a texture/sprite
	//------------------------------------------------------------------------------------------------------------------------
	void draw_texture(unsigned layer, const vec2_t& position, texture_handle_t texture, const core::scolor& tint, float rotation,
		const vec2_t& scale, program_handle_t program, const srenderstate& state, const vec2_t& origin, const core::srect& source)
	{
		SM_DRAW_CALL(4);

		//- Create a sprite draw command
		{
			core::cscope_mutex m(S_MUTEX);

			S_LAYERS[layer].m_commands.emplace_back().create([=]()
				{
// 					const auto& tm = *core::cservice_manager::find<ctexture_manager>();
// 					const auto& pm = *core::cservice_manager::find<cprogram_manager>();
// 					cvertices<spostexcoordcolor> vertices(vertex_type_transient, spostexcoordcolor::S_LAYOUT, spostexcoordcolor::S_HANDLE, 4, 0);
// 
// 					const auto& tex = tm.at(texture);
// 					const auto& prog = pm.at(program);
// 
// 					const float width = source.w() * scale.x;
// 					const float height = source.h() * scale.y;
// 
// 					//- Calculate the four corners of the quad.
// 					vec2_t topLeft = { position.x - origin.x * scale.x, position.y - origin.y * scale.y };
// 					vec2_t topRight = { topLeft.x + width, topLeft.y };
// 					vec2_t bottomLeft = { topLeft.x, topLeft.y + height };
// 					vec2_t bottomRight = { topLeft.x + width, topLeft.y + height };
// 
// 					//- Calculate rotation if required
// 					if (rotation != 0.0f)
// 					{
// 						// Apply rotation here (requires some math to rotate the points)
// 						// Assuming you have a `rotate_point` utility function:
// 						topLeft = math::rotate_point_around_origin(topLeft, position, rotation);
// 						topRight = math::rotate_point_around_origin(topRight, position, rotation);
// 						bottomLeft = math::rotate_point_around_origin(bottomLeft, position, rotation);
// 						bottomRight = math::rotate_point_around_origin(bottomRight, position, rotation);
// 					}
// 
// 					//- Set up vertices in clockwise order
// 					const auto c = tint.abgr();
// 					vertices.push(spostexcoordcolor::make(topLeft.x, topLeft.y, source.x(), source.y(), c));
// 					vertices.push(spostexcoordcolor::make(topRight.x, topRight.y, source.x() + source.w(), source.y(), c));
// 					vertices.push(spostexcoordcolor::make(bottomLeft.x, bottomLeft.y, source.x(), source.y() + source.h(), c));
// 					vertices.push(spostexcoordcolor::make(bottomRight.x, bottomRight.y, source.x() + source.w(), source.y() + source.h(), c));
// 
// 					//- Submit vertices to BGFX
// 					vertices.bind();
// 
// 					//- Set the texture
// 					bgfx::setTexture(0, tex.uniform().uniform(), tex.texture(), tex.flags());
// 
// 					//- Apply render state and submit draw call
// 					bgfx::setState(state.m_flags);
// 
// 					bgfx::submit(bgfx::ViewId{static_cast<uint16_t>(layer)}, prog.program());

					//- TODO: think of a better way to get actual shaders, textures etc. from resource managers,
					//- Note: these command are executed on main thread, but other threads may still post load/unload
					//- requests, so we have to account for that and make them thread-safe.
// 					const auto& tm = *core::cservice_manager::find<ctexture_manager>();
// 					const auto& sm = *core::cservice_manager::find<cshader_manager>();
// 
// 					//- texture and dimension
// 					const auto& _texture = tm.at(texture);
// 					const auto w = _texture.w();
// 					const auto h = _texture.h();
// 
// 					//- construct rectangles for where to sample from and where to draw
// 					raylib::Rectangle src = { source.x(), source.y(), source.w() * w, source.h() * h };
// 					raylib::Rectangle dst = { position.x, position.y, scale.x * w, scale.y * h };
// 					raylib::Vector2 orig = { origin.x, origin.y };
// 
// 					//- check some flags and do adjustments
// 					if (algorithm::bit_check(state.m_flags, renderable_flag_origin_center))
// 					{
// 						orig = { w * 0.5f, h * 0.5f };
// 					}
// 					if (algorithm::bit_check(state.m_flags, renderable_flag_origin_custom))
// 					{
// 						orig = { origin.x, origin.y };
// 					}
// 					if (algorithm::bit_check(state.m_flags, renderable_flag_flipx))
// 					{
// 						src.width = -src.width;
// 					}
// 					if (algorithm::bit_check(state.m_flags, renderable_flag_flipy))
// 					{
// 						src.height = -src.height;
// 					}
// 
// 					//- check if shader has changed since last draw command
// 					if (shader != S_CURRENT_SHADER)
// 					{
// 						S_CURRENT_SHADER = shader;
// 					}
// 
// 					//- setting shader is always enabled as we set default shader manually
// 					const auto& _shader = sm.at(S_CURRENT_SHADER);
// 					raylib::BeginShaderMode(_shader.shader());
// 
// 					//- check if renderstate has changed since last draw command
// 					if (state != S_CURRENT_RENDERSTATE)
// 					{
// 						S_CURRENT_RENDERSTATE = state;
// 					}
// 
// 					//- set blending mode only if enabled for this draw command
// 					if (algorithm::bit_check(state.m_flags, renderable_flag_blending_custom))
// 					{
// 						raylib::rlSetBlendMode(S_CURRENT_RENDERSTATE.m_blending.m_mode);
// 						raylib::rlSetBlendFactors(S_CURRENT_RENDERSTATE.m_blending.m_src_factor, S_CURRENT_RENDERSTATE.m_blending.m_dst_factor, S_CURRENT_RENDERSTATE.m_blending.m_equation);
// 						raylib::rlSetBlendMode(raylib::BLEND_CUSTOM);
// 					}
// 
// 					raylib::DrawTexturePro(_texture.texture(), src, dst, orig, rotation, tint.cliteral<raylib::Color>());
				});
		}
	}

} //- sm