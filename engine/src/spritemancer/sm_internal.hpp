#pragma once
#include "sm_resource_managers.hpp"
#include "sm_ui.hpp"

namespace sm
{
	class ccontext;

	//------------------------------------------------------------------------------------------------------------------------
	class ccontext
	{
	public:
		STATIC_INSTANCE(ccontext, s_ccontext);

		bool init(cwindow::sconfig& cfg);
		void shutdown();
		cwindow& window() const;
		void on_resize(unsigned w, unsigned h);

		//- Order of declaration is the expected order of calling
		void on_begin_drawing();
		void on_frame_end();
		void on_ui_frame();
		void on_ui_frame_end();
		void on_end_drawing();

		void begin_default();
		void submit(renderlayer_t layer, rendertarget_t target, core::srect rect);

		cspriteatlas_manager& am() { return *m_spriteatlas_manager; }
		const cspriteatlas_manager& am() const { return *m_spriteatlas_manager; }

		cshader_manager& sm() { return *m_shader_manager; }
		const cshader_manager& sm() const { return *m_shader_manager; }

		cmaterial_manager& mm() { return *m_material_manager; }
		const cmaterial_manager& mm() const { return *m_material_manager; }

		ctexture_manager& tm() { return *m_texture_manager; }
		const ctexture_manager& tm() const { return *m_texture_manager; }

		crendertarget_manager& rm() { return *m_rendertarget_manager; }
		const crendertarget_manager& rm() const { return *m_rendertarget_manager; }

	private:
		cspriteatlas_manager* m_spriteatlas_manager;
		cshader_manager* m_shader_manager;
		cmaterial_manager* m_material_manager;
		ctexture_manager* m_texture_manager;
		crendertarget_manager* m_rendertarget_manager;

		struct srendertargetdata
		{
			core::srect m_rect = { 0.0f, 0.0f, 0.0f, 0.0f };
			rendertarget_t m_target = 0;
		};

		map_t<renderlayer_t, srendertargetdata> m_renderpaths;
		core::cmutex m_mutex;

		rendertarget_t m_backbuffer;	//- default render target
		technique_t m_fxaa;				//- custom anti-aliasing shader
		material_t m_default;			//- default material
		technique_t m_sprite;			//- default pixel perfect shader
		bool m_has_fxaa;
		ptr_t<cwindow> m_mainwindow;

	private:
		ccontext();
		~ccontext();
	};

	//- shortcut
	ccontext& ctx();

} //- sm
