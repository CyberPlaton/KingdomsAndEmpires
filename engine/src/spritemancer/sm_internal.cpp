#include "sm_internal.hpp"
#include "sm_embedded_shaders.hpp"

namespace sm
{
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
		//- TODO: resize rendertargets

		cui::on_resize(w, h);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccontext::frame()
	{
		ZoneScopedN("ccontext::frame");

		//- render all renderpaths
		raylib::BeginDrawing();

		begin_default();

		{
			core::cscope_mutex m(m_mutex);

			for(const auto& path: m_renderpaths)
			{
				const auto& target = rm().get(path.second.m_target).target();
				const auto& rect = path.second.m_rect;

				if(!path.second.m_postprocess.empty())
				{
					//- draw with postprocess
					for(const auto& post: path.second.m_postprocess)
					{
						const auto& shader = sm().get(post);

						shader.bind();

						raylib::DrawTexturePro(target.texture, { 0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height },
							{ rect.x(), rect.y(), rect.w(), rect.h() }, {0.0f, 0.0f}, 0.0f, raylib::WHITE);

						shader.unbind();
					}
				}
				else
				{
					//- draw without postprocess
					raylib::DrawTexturePro(target.texture, { 0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height },
						{ rect.x(), rect.y(), rect.w(), rect.h() }, {0.0f, 0.0f}, 0.0f, raylib::WHITE);
				}
			}

			m_renderpaths.clear();
		}

		//- begin ImGui rendering
		cui::begin();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccontext::end()
	{
		ZoneScopedN("ccontext::end");
		
		//- end ImGui rendering and present everything
		cui::end();
	
		raylib::EndDrawing();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccontext::begin_default()
	{
		//- reset state to default before presenting backbuffer
		raylib::EndMode2D();
		raylib::EndBlendMode();
		raylib::EndTextureMode();

		raylib::BeginBlendMode(raylib::RL_BLEND_ALPHA);

		//- default clear of backbuffer
		raylib::ClearBackground(raylib::WHITE);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccontext::submit(renderlayer_t layer, rendertarget_t target, core::srect rect)
	{
		core::cscope_mutex m(m_mutex);

		m_renderpaths[layer] = { {}, rect, target };
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccontext::submit(renderlayer_t layer, rendertarget_t target, core::srect rect, vector_t<technique_t> postprocess)
	{
		core::cscope_mutex m(m_mutex);

		m_renderpaths[layer] = { postprocess, rect, target };
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
