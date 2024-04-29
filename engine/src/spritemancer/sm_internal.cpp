#include "sm_internal.hpp"
#include "sm_embedded_shaders.hpp"

namespace sm
{
	namespace
	{
		constexpr stringview_t C_RENDERTARGET_NAME = "RenderTargetDefault";

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	ccontext& ctx()
	{
		return ccontext::instance();
	}

	//- Responsible for creating main window and OpenGL context,
	//- initialzing ImGui ui (when required), and loading default or basic rendering related resources.
	//------------------------------------------------------------------------------------------------------------------------
	bool ccontext::init(cwindow::sconfig& cfg)
	{
		m_mainwindow = std::make_unique<cwindow>(cfg);

		//- imgui ui has to be initialized statically
		//- TODO: decide whether exclude from release builds when a gameplay UI solution is prepared
		if (!cui::init())
		{
			//- report failure, depending on whether we expect using ImGui
			return false;
		}

		cui::on_resize(cfg.m_width, cfg.m_height);

		//- default framebuffer
		if (m_backbuffer = rm().load(C_RENDERTARGET_NAME.data(), cfg.m_width, cfg.m_height); !algorithm::is_valid_handle(m_backbuffer))
		{
			//- report failure
			return false;
		}

		//- default sprite shader
		if (m_sprite = sm().load_from_memory(C_SPRITE_TECHNIQUE_NAME.data(), programs::pixelperfect::s_vs, programs::pixelperfect::s_ps);
			!algorithm::is_valid_handle(m_sprite))
		{
			//- report failure
			return false;
		}

		//- default material
		if (m_default = mm().load(C_DEFAULT_MATERIAL_NAME.data(), m_sprite, blending_mode_alpha, blending_equation_blend_color,
			blending_factor_src_color, blending_factor_one_minus_src_alpha);
			!algorithm::is_valid_handle(m_default))
		{
			//- report failure
			return false;
		}

		//- anti-aliasing shader
		if (algorithm::bit_on(cfg.m_flags, window_flag_fxaa))
		{
			m_fxaa = sm().load_from_memory(C_FXAA_TECHNIQUE_NAME.data(), 0, programs::fxaa::s_ps);

			if (!algorithm::is_valid_handle(m_fxaa))
			{
				//- could not load fxaa shader, report a warning and disable it
				m_has_fxaa = false;
			}
			m_has_fxaa = true;
		}
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccontext::shutdown()
	{
		m_mainwindow.reset();
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::cwindow& ccontext::window() const
	{
		ASSERT(m_mainwindow, "Invalid operation. Window was not created! Make sure to initialize context first");
		return *m_mainwindow.get();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccontext::on_resize(unsigned w, unsigned h)
	{
		rm().modify(m_backbuffer).resize(w, h);

		cui::on_resize(w, h);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccontext::on_begin_drawing()
	{
		//- TODO: consider changing this up a bit. As of now this is only required to draw the ImGui UI to
		//- our default backbuffer. Having a game UI won´t use ImGui and this most certainly
		rm().get(m_backbuffer).bind();

		begin_default();

		//- part where rendering in world space ought to be done
		//- originally the renderpass_stack part
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccontext::on_frame_end()
	{
	}

	void ccontext::on_ui_frame()
	{
		cui::begin();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccontext::on_ui_frame_end()
	{
		cui::end();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccontext::on_end_drawing()
	{
		ZoneScopedN("ccontext::on_end_drawing");

		//- reset state to default before presenting backbuffer
		raylib::EndMode2D();
		raylib::EndBlendMode();
		raylib::EndTextureMode();

		//- render submitted rendertargets in layered order to default backbuffer
		const auto& backbuffer = rm().get(m_backbuffer);
		{
			core::cscope_mutex m(m_mutex);

			backbuffer.bind();
			raylib::BeginBlendMode(raylib::RL_BLEND_ALPHA);

			for (const auto& pair : m_renderpaths)
			{
				const auto& target = rm().get(pair.second.m_target).target();

				const auto& rect = pair.second.m_rect;

				raylib::DrawTexturePro(target.texture, { 0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height },
					{ rect.x(), rect.y(), rect.w(), rect.h() }, {0.0f, 0.0f}, 0.0f, raylib::WHITE);

				raylib::DrawTextureRec(target.texture,
					{ 0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height}, {0.0f, 0.0f}, to_cliteral(C_COLOR_WHITE));
			}

			raylib::EndBlendMode();
			backbuffer.unbind();

			m_renderpaths.clear();
		}


		//- Present default rendertarget to screen
		raylib::BeginDrawing();

		if (m_has_fxaa)
		{
			const auto& technique = sm().get(m_fxaa);
			const auto& rendertarget = backbuffer.target();
			const auto resolution = vec2_t(rendertarget.texture.width, rendertarget.texture.height);

			raylib::BeginShaderMode(technique.shader());

			raylib::SetShaderValue(technique.shader(), raylib::GetShaderLocation(technique.shader(), C_FXAA_TECHNIQUE_RESOLUTION_PROP.data()),
				&resolution, raylib::SHADER_UNIFORM_VEC2);
		}

		const auto w = SCAST(float, backbuffer.target().texture.width);
		const auto h = SCAST(float, backbuffer.target().texture.height);

		raylib::DrawTextureRec(backbuffer.target().texture, { 0.0f, 0.0f, w, -h }, { 0.0f, 0.0f }, to_cliteral(C_COLOR_WHITE));

		if (m_has_fxaa)
		{
			raylib::EndShaderMode();
		}

		raylib::EndDrawing();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccontext::begin_default()
	{
		raylib::BeginBlendMode(raylib::RL_BLEND_ALPHA);

		//- default clear of backbuffer
		raylib::ClearBackground(raylib::WHITE);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccontext::submit(renderlayer_t layer, rendertarget_t target, core::srect rect)
	{
		core::cscope_mutex m(m_mutex);

		m_renderpaths[layer] = { rect, target };
	}

	//------------------------------------------------------------------------------------------------------------------------
	ccontext::ccontext() :
		m_fxaa(invalid_handle_t), m_default(invalid_handle_t), m_sprite(invalid_handle_t),
		m_has_fxaa(false), m_mainwindow(nullptr)
	{
		m_spriteatlas_manager = core::cservice_manager::find<cspriteatlas_manager>();
		m_shader_manager = core::cservice_manager::find<cshader_manager>();
		m_material_manager = core::cservice_manager::find<cmaterial_manager>();
		m_texture_manager = core::cservice_manager::find<ctexture_manager>();
		m_rendertarget_manager = core::cservice_manager::find<crendertarget_manager>();
	}

	//------------------------------------------------------------------------------------------------------------------------
	ccontext::~ccontext()
	{
	}

} //- sm
