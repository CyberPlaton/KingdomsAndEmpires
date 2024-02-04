#pragma once
#include "sm_renderer.hpp"
#include "sm_ui.hpp"

namespace sm
{
	namespace internal
	{
		//------------------------------------------------------------------------------------------------------------------------
		class ccontext
		{
		public:
			STATIC_INSTANCE(ccontext, s_ccontext);
			ccontext() = default;
			~ccontext() = default;

			bool init(cwindow::sconfig& cfg);
			void shutdown();

			crenderer& renderer() const;
			cwindow& window() const;

			void on_resize(unsigned w, unsigned h);

			//- render functions.
			//- order of declaration is expected order of calling
			void on_begin_drawing(ccamera* camera);
			void on_frame_end();
			void on_ui_frame();
			void on_ui_frame_end();
			void on_end_drawing();

			//- render utility
			void begin_default_render_target();
			void begin_default_blend_mode();
			void begin_blend_mode(blending_mode mode);
			void end_blend_mode();
			void begin_render_target(render_target_t& texture);
			void end_render_target(raylib::Shader combine_technique = {0});

			//- utility
			bool is_platform_windows() const;
			bool is_platform_linux() const;
			bool is_platform_macosx() const;
			bool is_platform_ios() const;
			bool is_platform_android() const;
			bool is_platform_xboxone() const;
			bool is_platform_ps() const;
			bool is_platform_nx() const;

		private:
			raylib::RenderTexture2D m_default_rendertarget;
			raylib::Shader m_msaa_rendertarget_technique;

			ptr_t<crenderer> m_renderer = nullptr;
			ptr_t<cwindow> m_window = nullptr;

			vec2_t m_screen_resolution = {0.0f, 0.0f};
			bool m_msaa_enabled = false;

			ccamera* m_frame_camera = nullptr;

		private:
			void end_default_render_target();
		};
	}


	//- shortcut
	internal::ccontext& ctx();
}