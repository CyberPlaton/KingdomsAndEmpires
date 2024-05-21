#include "sm_renderer_bgfx.hpp"
#include "../sm_embedded_shaders.hpp"

namespace sm
{
	namespace vertex_layouts
	{
		//------------------------------------------------------------------------------------------------------------------------
		struct sposcolortexcoord
		{
			static bool init();
			static sposcolortexcoord make(float x, float y, const core::scolor& color, float u, float v);

			inline static bgfx::VertexLayout S_LAYOUT;
			inline static bgfx::VertexLayoutHandle S_LAYOUT_HANDLE;

			vec3_t m_position;
			uint32_t m_abgr;
			vec2_t m_uv;
		};

		//------------------------------------------------------------------------------------------------------------------------
		bool sposcolortexcoord::init()
		{
			S_LAYOUT.begin(bgfx::getRendererType())
				.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
				.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
				.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
				.end();

			S_LAYOUT_HANDLE = bgfx::createVertexLayout(S_LAYOUT);

			return bgfx::isValid(S_LAYOUT_HANDLE);
		}

		//------------------------------------------------------------------------------------------------------------------------
		sposcolortexcoord sposcolortexcoord::make(float x, float y, const core::scolor& color, float u, float v)
		{
			return { vec3_t(x, y, 0.0f), color.abgr(), vec2_t(u, v) };
		}

		//------------------------------------------------------------------------------------------------------------------------
		struct spostexcoord
		{
			static bool init();
			static spostexcoord make(float x, float y, float u, float v);

			inline static bgfx::VertexLayout S_LAYOUT;
			inline static bgfx::VertexLayoutHandle S_LAYOUT_HANDLE;

			vec3_t m_position;
			vec2_t m_uv;
		};

		//------------------------------------------------------------------------------------------------------------------------
		bool spostexcoord::init()
		{
			S_LAYOUT.begin(bgfx::getRendererType())
				.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
				.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
				.end();

			S_LAYOUT_HANDLE = bgfx::createVertexLayout(S_LAYOUT);

			return bgfx::isValid(S_LAYOUT_HANDLE);
		}

		//------------------------------------------------------------------------------------------------------------------------
		spostexcoord spostexcoord::make(float x, float y, float u, float v)
		{
			return { vec3_t(x, y, 0.0f), vec2_t(u, v) };
		}

	} //- vertex_layouts

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

	using quad_vertex_t = vertex_layouts::sposcolortexcoord;
	using layer_quad_vertex_t = vertex_layouts::spostexcoord;

	namespace
	{
		static uint16_t S_W = 0;
		static uint16_t S_H = 0;
		static uint16_t S_X = 0;
		static uint16_t S_Y = 0;
		static unsigned S_BLENDING_MODE = BGFX_STATE_DEFAULT;
		static cshader S_DECAL_VS_DEFAULT;
		static cshader S_DECAL_FS_DEFAULT;
		static cprogram S_DECAL_PROGRAM_DEFAULT;
		static cshader S_LAYER_VS_DEFAULT;
		static cshader S_LAYER_PS_DEFAULT;
		static cprogram S_LAYER_PROGRAM_DEFAULT;
		static srenderable S_BLANK_QUAD;
		static core::scolor S_WHITE = { 250, 250, 250, 250 };
		constexpr stringview_t C_TEXTURE_UNIFORM_NAME = "s_tex";
		static bgfx::UniformHandle S_TEXTURE_UNIFORM;

		constexpr unsigned C_STATE_DEFAULT = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_CULL_CCW | BGFX_STATE_MSAA;
		constexpr unsigned C_CLEAR_WITH_DEPTH = BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH;
		constexpr unsigned C_CLEAR_NO_DEPTH = BGFX_CLEAR_COLOR;
		constexpr bgfx::ViewId C_VIEW_DEFAULT = 0;
		constexpr mat4_t C_VIEWMAT_DEFAULT = mat4_t(1.0f);
		constexpr mat4_t C_PROJMAT_DEFAULT = mat4_t(1.0f);

		#define C_VERTICES_COUNT_MAX 128
		static unsigned S_VERTEX_COUNT = 0;
		static array_t<quad_vertex_t, C_VERTICES_COUNT_MAX> S_VERTICES; //- Note: for one immediate draw call. Not batched.
		static bgfx::TransientVertexBuffer S_TVB;

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

		//- report success and continue reporting capabilities. Implement if required
		//const auto* caps = bgfx::getCaps();

#if CORE_DEBUG
		bgfx::setDebug(BGFX_DEBUG_TEXT | BGFX_DEBUG_STATS);
#endif

		//- set up used vertex type layouts
		if (!vertex_layouts::spostexcoord::init())
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Failed creating 'spostexcoord' vertex layout");
			}
			return opresult_fail;
		}
		if (!vertex_layouts::sposcolortexcoord::init())
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Failed creating 'sposcolortexcoord' vertex layout");
			}
			return opresult_fail;
		}

		//- set up default vertex and fragment shaders
		if (const auto result = S_DECAL_VS_DEFAULT.load_from_memory(shader_type_vertex, &shaders::sprite::vs_sprite_dx11[0], sizeof(shaders::sprite::vs_sprite_dx11));
			result != opresult_ok)
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Failed creating default vertex shader");
			}
			return opresult_fail;
		}
		if (const auto result = S_DECAL_FS_DEFAULT.load_from_memory(shader_type_fragment, &shaders::sprite::ps_sprite_dx11[0], sizeof(shaders::sprite::ps_sprite_dx11));
			result != opresult_ok)
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Failed creating default fragment shader");
			}
			return opresult_fail;
		}
		if (const auto result = S_DECAL_PROGRAM_DEFAULT.load_from_shaders(S_DECAL_VS_DEFAULT, S_DECAL_FS_DEFAULT);
			result != opresult_ok)
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Failed creating default decal program");
			}
			return opresult_fail;
		}
		if (const auto result = S_LAYER_VS_DEFAULT.load_from_memory(shader_type_vertex, &shaders::sprite::vs_sprite_dx11[0], sizeof(shaders::sprite::vs_sprite_dx11));
			result != opresult_ok)
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Failed creating default layer quad vertex shader");
			}
			return opresult_fail;
		}
		if (const auto result = S_LAYER_PS_DEFAULT.load_from_memory(shader_type_fragment, &shaders::sprite::ps_sprite_dx11[0], sizeof(shaders::sprite::ps_sprite_dx11));
			result != opresult_ok)
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Failed creating default layer quad fragment shader");
			}
			return opresult_fail;
		}
		if (const auto result = S_LAYER_PROGRAM_DEFAULT.load_from_shaders(S_LAYER_VS_DEFAULT, S_LAYER_PS_DEFAULT);
			result != opresult_ok)
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Failed creating default layer quad program");
			}
			return opresult_fail;
		}

		//- create a blank texture for decals without a texture set
		S_BLANK_QUAD.m_image.create_solid(1, 1, S_WHITE);
		S_BLANK_QUAD.m_texture.load_from_image(S_BLANK_QUAD.m_image);
		S_TEXTURE_UNIFORM = bgfx::createUniform(C_TEXTURE_UNIFORM_NAME.data(), bgfx::UniformType::Sampler);

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult crenderer_bgfx::shutdown_device()
	{
		//- destroy all used resources
		cprogram::destroy(S_DECAL_PROGRAM_DEFAULT);
		ctexture::destroy(S_BLANK_QUAD.m_texture);
		cimage::destroy(S_BLANK_QUAD.m_image);

		bgfx::shutdown();

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::prepare_frame()
	{
		S_VERTEX_COUNT = 0;

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

		static bgfx::ProgramHandle invalid; invalid.idx = bgfx::kInvalidHandle;

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
		bgfx::setViewClear(C_VIEW_DEFAULT, depth ? C_CLEAR_WITH_DEPTH : C_CLEAR_NO_DEPTH, color.abgr(), 1.0f, 0);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::blendmode(blending_mode mode)
	{
		//- TODO
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::bind_texture(uint64_t id)
	{
		bgfx::setTexture(0, S_TEXTURE_UNIFORM, { static_cast<uint16_t>(id) });
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::render_layer_quad(const vec2_t& position, const vec2_t& size, const core::scolor& color)
	{
		//- TODO: unclear what the original purpose of this is... maybe it was simulating framebuffers?!
		//- If so, then we have to consider how to deal with framebuffers...

		quad_vertex_t vertices[4] =
		{
			quad_vertex_t::make(-1.0f, -1.0f,	color, 0.0f * size.x + position.x, 1.0f * size.y + position.y),
			quad_vertex_t::make(1.0f, -1.0f,	color, 1.0f * size.x + position.x, 1.0f * size.y + position.y),
			quad_vertex_t::make(-1.0f, 1.0f,	color, 0.0f * size.x + position.x, 0.0f * size.y + position.y),
			quad_vertex_t::make(1.0f, 1.0f,		color, 1.0f * size.x + position.x, 0.0f * size.y + position.y)
		};

		//- create and set vertex buffer
		if (4 <= bgfx::getAvailTransientVertexBuffer(4, quad_vertex_t::S_LAYOUT))
		{
			bgfx::allocTransientVertexBuffer(&S_TVB, 4, quad_vertex_t::S_LAYOUT);

			bx::memCopy((quad_vertex_t*)S_TVB.data, &vertices[0], 4 * sizeof(quad_vertex_t));

			bgfx::setVertexBuffer(0, S_TVB.handle, 0, 4, quad_vertex_t::S_LAYOUT_HANDLE);
		}

		//- set state for primitive
		bgfx::setState(C_STATE_DEFAULT);

		//- submit primitive with program
		bgfx::submit(C_VIEW_DEFAULT, S_LAYER_PROGRAM_DEFAULT.handle());
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_bgfx::render_decal(const sdecal& decal)
	{
		blendmode(decal.m_blending);

		const auto id = bgfx::isValid(decal.m_texture)? decal.m_texture : S_BLANK_QUAD.m_texture.handle();

		for (auto i = 0u; i < decal.m_points; ++i)
		{
			S_VERTICES[i] = quad_vertex_t::make(decal.m_position[i].x, decal.m_position[i].y,
				decal.m_tint[i], decal.m_uv[i].x, decal.m_uv[i].y);
		}

		S_VERTEX_COUNT += decal.m_points;

		//- create and set vertex buffer
		if (S_VERTEX_COUNT <= bgfx::getAvailTransientVertexBuffer(S_VERTEX_COUNT, quad_vertex_t::S_LAYOUT))
		{
			bgfx::allocTransientVertexBuffer(&S_TVB, S_VERTEX_COUNT, quad_vertex_t::S_LAYOUT);

			bx::memCopy((quad_vertex_t*)S_TVB.data, &S_VERTICES[0], S_VERTEX_COUNT * sizeof(quad_vertex_t));

			bgfx::setVertexBuffer(0, S_TVB.handle, 0, S_VERTEX_COUNT, quad_vertex_t::S_LAYOUT_HANDLE);
		}

		//- create and set index buffer


		//- create and set camera transform and projection
		static float pitch = 0.0f;
		static float yaw = 0.0f;
		float cam_rotation[16];
		bx::mtxRotateXYZ(cam_rotation, pitch, yaw, 0.0f);

		float cam_translation[16];
		bx::mtxTranslate(cam_translation, 0.0f, 0.0f, -5.0f);

		float cam_transform[16];
		bx::mtxMul(cam_transform, cam_translation, cam_rotation);

		float view[16];
		bx::mtxInverse(view, cam_transform);

		float proj[16];
		bx::mtxProj(
			proj, 60.0f, float(S_W) / float(S_H), 0.1f,
			100.0f, bgfx::getCaps()->homogeneousDepth);

		bgfx::setViewTransform(C_VIEW_DEFAULT, view, proj);

		//- set model transform
		float model[16];
		bx::mtxIdentity(model);
		bgfx::setTransform(model);

		//- set texture and texture sampler
		bgfx::setTexture(0, S_TEXTURE_UNIFORM, decal.m_texture);

		//- set state for primitive
		bgfx::setState(C_STATE_DEFAULT);

		//- submit primitive with program
		bgfx::submit(C_VIEW_DEFAULT, S_DECAL_PROGRAM_DEFAULT.handle());
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