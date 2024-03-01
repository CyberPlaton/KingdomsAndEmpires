#include "sm_internal.hpp"
#include <core.h>

namespace sm
{
	namespace
	{
		//------------------------------------------------------------------------------------------------------------------------
		raylib::Color ccolor(const core::scolor& c)
		{
			return { c.r(), c.g() ,c.b(), c.a() };
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	internal::ccontext& ctx()
	{
		return internal::ccontext::instance();
	}

	namespace internal
	{
		//------------------------------------------------------------------------------------------------------------------------
		bool ccontext::init(cwindow::sconfig& cfg)
		{
			m_window = std::make_unique<cwindow>(cfg);
			m_renderer = std::make_unique<crenderer>();

			//- imgui ui has to be initialized statically
			//- TODO: decide whether exclude from release builds when a gameplay UI solution
			//- is prepared

			if (cui::init())
			{
				if(m_default_rendertarget = raylib::LoadRenderTexture(cfg.m_width, cfg.m_height); raylib::IsRenderTextureReady(m_default_rendertarget))
				{
					cui::on_resize(cfg.m_width, cfg.m_height);

					//- MSAA
					if(algorithm::bit_on(cfg.m_flags, window_flag_msaa))
					{
						//- TODO: Make ps_fxaa an embedded shader
						if(m_msaa_rendertarget_technique = raylib::LoadShader(0, "resources/shaders/ps_fxaa.ps"); raylib::IsShaderReady(m_msaa_rendertarget_technique))
						{
							m_msaa_enabled = true;
						}
					}

					//- finalize
					m_screen_resolution = { cfg.m_width, cfg.m_height };
					return true;
				}
			}
			return false;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccontext::shutdown()
		{
			m_renderer.reset();
			m_window.reset();

			if (raylib::IsShaderReady(m_msaa_rendertarget_technique))
			{
				raylib::UnloadShader(m_msaa_rendertarget_technique);
			}
			if (raylib::IsRenderTextureReady(m_default_rendertarget))
			{
				raylib::UnloadRenderTexture(m_default_rendertarget);
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		sm::crenderer& ccontext::renderer() const
		{
			ASSERT(m_renderer, "Invalid operation. Renderer was not created! Make sure to initialize context first");
			return *m_renderer.get();
		}

		//------------------------------------------------------------------------------------------------------------------------
		sm::cwindow& ccontext::window() const
		{
			ASSERT(m_window, "Invalid operation. Window was not created! Make sure to initialize context first");
			return *m_window.get();
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccontext::on_resize(unsigned w, unsigned h)
		{
			if (raylib::IsRenderTextureReady(m_default_rendertarget))
			{
				raylib::UnloadRenderTexture(m_default_rendertarget);
			}
			m_default_rendertarget = raylib::LoadRenderTexture(static_cast<int>(w), static_cast<int>(h));
			m_screen_resolution = { static_cast<float>(w), static_cast<float>(h) };

			sm::cui::on_resize(w, h);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccontext::on_begin_drawing(ccamera* camera)
		{
			begin_default_render_target();
			begin_default_blend_mode();
			raylib::ClearBackground(ccolor(camera->clearcolor()));
			camera->use();
			m_frame_camera = camera;

			//- part where rendering in world space ought to be done
			//- originally the renderpass_stack part
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccontext::on_frame_end()
		{
			m_frame_camera->end();
			end_blend_mode();
			end_default_render_target();
		}

		void ccontext::on_ui_frame()
		{
			begin_default_render_target();
			cui::begin();
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccontext::on_ui_frame_end()
		{
			cui::end();
			end_default_render_target();
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccontext::on_end_drawing()
		{
			raylib::BeginDrawing();

			//- check whether we should have MSAA
			if (m_msaa_enabled)
			{
				raylib::BeginShaderMode(m_msaa_rendertarget_technique);
				raylib::SetShaderValue(m_msaa_rendertarget_technique,
					raylib::GetShaderLocation(m_msaa_rendertarget_technique, "resolution"), &m_screen_resolution, raylib::SHADER_UNIFORM_VEC2);
			}

			raylib::DrawTextureRec(m_default_rendertarget.texture,
				{ 0.0f, 0.0f, (float)m_default_rendertarget.texture.width, (float)-m_default_rendertarget.texture.height },
				{ 0.0f, 0.0f }, {250, 250, 250, 255});

			if (m_msaa_enabled)
			{
				raylib::EndShaderMode();
			}

			raylib::EndDrawing();
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccontext::begin_default_render_target()
		{
			raylib::BeginTextureMode(m_default_rendertarget);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccontext::begin_default_blend_mode()
		{
			raylib::BeginBlendMode(raylib::BLEND_ALPHA);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccontext::begin_blend_mode(blending_mode mode)
		{
			raylib::BeginBlendMode(mode);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccontext::end_blend_mode()
		{
			raylib::EndBlendMode();
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccontext::begin_render_target(render_target_t& texture)
		{
			//- TODO
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccontext::end_render_target(raylib::Shader combine_technique /*= {0}*/)
		{
			//- TODO
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccontext::end_default_render_target()
		{
			raylib::EndTextureMode();
		}

	}
}