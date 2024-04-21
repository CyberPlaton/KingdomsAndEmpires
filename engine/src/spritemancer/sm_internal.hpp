#pragma once
#include "sm_resource_managers.hpp"
#include "sm_ui.hpp"

namespace sm
{
	namespace detail
	{
		//- Base class for renderer. Automatically submits command buffer when going out of scope.
		//------------------------------------------------------------------------------------------------------------------------
		class irenderer
		{
		public:
			irenderer() = default;
			virtual ~irenderer();

		protected:
			inline ccommandbuffer& buffer() { return m_commands; }

		private:
			ccommandbuffer m_commands;

		private:
			void submit();
		};

	} //- detail

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
		void on_begin_drawing(ccamera* camera);
		void on_frame_end();
		void on_ui_frame();
		void on_ui_frame_end();
		void on_end_drawing();

		void begin_default_blend_mode();
		void begin_blend_mode(blending_mode mode);
		void end_blend_mode();
		void begin_render_target(rendertarget_t texture);
		void end_render_target();
		void push_commands(vector_t<cdrawcommand>&& buffer);

		cspriteatlas_manager& am() { return m_spriteatlas_manager; }
		const cspriteatlas_manager& am() const { return m_spriteatlas_manager; }

		cshader_manager& sm() { return m_shader_manager; }
		const cshader_manager& sm() const { return m_shader_manager; }

		cmaterial_manager& mm() { return m_material_manager; }
		const cmaterial_manager& mm() const { return m_material_manager; }

		ctexture_manager& tm() { return m_texture_manager; }
		const ctexture_manager& tm() const { return m_texture_manager; }

		crendertarget_manager& rm() { return m_rendertarget_manager; }
		const crendertarget_manager& rm() const { return m_rendertarget_manager; }

	private:
		cspriteatlas_manager m_spriteatlas_manager;
		cshader_manager m_shader_manager;
		cmaterial_manager m_material_manager;
		ctexture_manager m_texture_manager;
		crendertarget_manager m_rendertarget_manager;

		//- World space position of a 2D object after transforming
		struct sdecomposed_transform
		{
			sdecomposed_transform(float x, float y, float w, float h, float r);

			float m_x, m_y, m_w, m_h, m_rotation;
		};

		//- Final draw command for a sprite ready to be drawn
		struct ssprite_drawcommand
		{
			ssprite_drawcommand(core::srect rect, sdecomposed_transform transform,
				core::scolor color, material_t material, texture_t texture);

			core::srect m_rect;
			sdecomposed_transform m_transform;
			core::scolor m_color;
			material_t m_material;
			texture_t m_texture;
		};

		umap_t<renderlayer_t, vector_t<ssprite_drawcommand>> m_drawcommands;

		crendertarget m_backbuffer; //- default render target
		technique_t m_fxaa;			//- custom anti-aliasing shader
		material_t m_default;		//- default material
		technique_t m_sprite;		//- default pixel perfect shader
		bool m_has_fxaa;
		ptr_t<cwindow> m_mainwindow = nullptr;
		ccamera* m_frame_camera = nullptr;

	private:
		ccontext();
		~ccontext();
	};

	//- shortcut
	ccontext& ctx();

} //- sm