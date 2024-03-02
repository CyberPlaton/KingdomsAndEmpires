#pragma once
#include "sm_renderer.hpp"
#include "sm_resource_managers.hpp"
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

			cspriteatlas_manager& am() {return m_spriteatlas_manager;}
			const cspriteatlas_manager& am() const {return m_spriteatlas_manager;}

			ctechnique_manager& sm() { return m_technique_manager; }
			const ctechnique_manager& sm() const { return m_technique_manager; }

			cmaterial_manager& mm() { return m_material_manager; }
			const cmaterial_manager& mm() const { return m_material_manager; }

			ctexture_manager& tm() { return m_texture_manager; }
			const ctexture_manager& tm() const { return m_texture_manager; }

		private:
			cspriteatlas_manager m_spriteatlas_manager;
			ctechnique_manager m_technique_manager;
			cmaterial_manager m_material_manager;
			ctexture_manager m_texture_manager;

			raylib::RenderTexture2D m_default_rendertarget;
			raylib::Shader m_msaa_rendertarget_technique;

			ptr_t<crenderer> m_renderer = nullptr;
			ptr_t<cwindow> m_window = nullptr;

			vec2_t m_screen_resolution = {0.0f, 0.0f};
			bool m_msaa_enabled = false;

			ccamera* m_frame_camera = nullptr;

		private:
			ccontext();
			~ccontext();

			void end_default_render_target();
		};

	} //- internal

	//- shortcut
	internal::ccontext& ctx();

} //- sm