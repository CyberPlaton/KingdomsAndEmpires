#pragma once
#include "sm_common.hpp"

namespace sm
{
	class crenderer;

	namespace detail
	{
		//------------------------------------------------------------------------------------------------------------------------
		class cspriteatlas_manager
		{
			friend class sm::crenderer;

			struct sspriteatlas_wrapper
			{
				csprite_atlas m_atlas;
				bool m_removed = false;
			};

		public:
			cspriteatlas_manager(crenderer& renderer);
			~cspriteatlas_manager();

			spriteatlas_t create_spriteatlas(const string_t& spriteatlas_name, const string_t& texture_path, const vec2_t& frames);
			spriteatlas_t get_spriteatlas(const string_t& spriteatlas_name) const;
			void unload_spriteatlas(spriteatlas_t handle);
			core::srect get_subtexture(spriteatlas_t atlas, const string_t& subtexture_name);

		private:
			vector_t< sspriteatlas_wrapper > m_spriteatlas_manager;
			umap_t<unsigned, spriteatlas_t > m_spriteatlas_names;
			unsigned m_spriteatlases_count;

			bool m_fragmented;
			unsigned m_fragmentation_count;
			crenderer& m_renderer;

			const csprite_atlas& get_internal_spriteatlas(spriteatlas_t handle);
		};

		//------------------------------------------------------------------------------------------------------------------------
		class ctechnique_manager
		{
			friend class sm::crenderer;

			struct sshader_wrapper
			{
				sshader_wrapper(raylib::Shader shader) : m_shader(shader) {}

				raylib::Shader m_shader;
				bool m_removed = false;
			};
		public:
			ctechnique_manager();
			~ctechnique_manager();

			technique_t create_technique(const string_t& technique_name, const string_t& vs_path, const string_t& ps_path);
			technique_t create_technique_embedded(const string_t& technique_name, const char* vs, const char* ps);
			technique_t get_technique(const string_t& technique_name);
			void unload_technique(technique_t handle);

		private:
			vector_t<sshader_wrapper> m_technique_manager;
			umap_t <unsigned, technique_t> m_technique_names;
			unsigned m_technique_count;

			bool m_fragmented;
			unsigned m_fragmentation_count;

		private:
			const raylib::Shader& raylib_shader(technique_t handle) const;
		};

		//------------------------------------------------------------------------------------------------------------------------
		class cmaterial_manager
		{
			friend class sm::crenderer;

			struct smaterial_wrapper
			{
				smaterial_wrapper(cmaterial material) : m_material(std::move(material)) {}

				cmaterial m_material;
				bool m_removed = false;
			};

		public:
			cmaterial_manager();
			~cmaterial_manager();

			material_t create_material(const string_t& material_name, technique_t technique,
				blending_mode mode = blending_mode_alpha, blending_equation equation = blending_equation_blend_color,
				blending_factor src = blending_factor_src_color, blending_factor dst = blending_factor_one_minus_src_alpha);
			bool create_default_material(technique_t technique,
				blending_mode mode = blending_mode_alpha, blending_equation equation = blending_equation_blend_color,
				blending_factor src = blending_factor_src_color, blending_factor dst = blending_factor_one_minus_src_alpha);
			material_t get_material(const string_t& material_name) const;
			void unload_material(material_t handle);
			material_t default_material() const;

		private:
			vector_t< smaterial_wrapper > m_material_manager;
			umap_t < unsigned, material_t > m_material_names;
			unsigned m_material_count;
			const material_t m_default_material = 0;

			bool m_fragmented;
			unsigned m_fragmentation_count;

		private:
			const cmaterial& get_internal_material(material_t handle) const;
		};

		//------------------------------------------------------------------------------------------------------------------------
		class ctexture_manager
		{
			friend class sm::crenderer;

			struct stexture_wrapper
			{
				stexture_wrapper(raylib::Texture2D texture) : m_texture(texture) {}

				raylib::Texture2D m_texture;
				bool m_removed = false;
			};
		public:
			ctexture_manager();
			~ctexture_manager();

			void unload_texture(texture_t handle);
			texture_t create_texture(const string_t& texture_name, const string_t& path);
			texture_t get_texture(const string_t& texture_name) const;
			const raylib::Texture2D* get_gl_texture(const string_t& texture_name) const;

		private:
			vector_t<stexture_wrapper> m_texture_manager;
			umap_t<unsigned, texture_t> m_texture_names;
			unsigned m_texture_count;

			bool m_fragmented;
			unsigned m_fragmentation_count;

		private:
			const raylib::Texture2D& get_raylib_texture(texture_t handle) const;
		};

	} //- detail

	//------------------------------------------------------------------------------------------------------------------------
	class crenderer
	{
	public:
		crenderer();
		~crenderer();

		//- present submitted sprites and reset state
		void sort_and_submit();
		
		//- make sure everything is ready for next submission start
		void touch();

		//- sprite renderer part
		void draw_sprite(unsigned layer, const mat4_t& transform, texture_t texture, material_t material, const core::srect& rect, const core::scolor& color, bool flipx, bool flipy);
		void draw_sprite(unsigned layer, const vec2_t& position, float rotation, const vec2_t& scale, texture_t texture, material_t material, const core::srect& rect, const core::scolor& color, bool flipx, bool flipy);
		
		//- native sprite atlas support
		void draw_sprite_atlas_sprite(unsigned layer, const vec2_t& position, float rotation, const vec2_t& scale, spriteatlas_t atlas, subtexture_t subtexture, const core::scolor& color, bool flipx, bool flipy);
		void draw_sprite_atlas_sprite(unsigned layer, const mat4_t& transform, spriteatlas_t atlas, subtexture_t subtexture, const core::scolor& color, bool flipx, bool flipy);
		
		//- resource management
		material_t create_material(const string_t& material_name, technique_t technique,
			blending_mode mode = blending_mode_alpha, blending_equation equation = blending_equation_blend_color,
			blending_factor src = blending_factor_src_color, blending_factor dst = blending_factor_one_minus_src_alpha);
		material_t get_material(const string_t& material_name) const;
		void unload_material(material_t handle);
		texture_t create_texture(const string_t& texture_name, const string_t& path);
		texture_t get_texture(const string_t& texture_name) const;
		void unload_texture(texture_t handle);
		const raylib::Texture2D* get_gl_texture(const string_t& texture_name) const;

		technique_t create_technique(const string_t& technique_name, const string_t& vs_path, const string_t& ps_path);
		technique_t create_technique_embedded(const string_t& technique_name, const char* vs, const char* ps);
		technique_t get_technique(const string_t& technique_name);
		void unload_technique(technique_t handle);
		spriteatlas_t create_spriteatlas(const string_t& spriteatlas_name, const string_t& texture_path, const vec2_t& frame_size);
		spriteatlas_t get_spriteatlas(const string_t& spriteatlas_name) const;
		void unload_spriteatlas(spriteatlas_t handle);
		core::srect get_subtexture(spriteatlas_t atlas, const string_t& subtexture_name);
		
		//- utility
		vec2_t get_texture_size(texture_t texture);

	private:
		struct ssprite_drawcommand
		{
			unsigned m_index		= MAX(unsigned);
			texture_t m_texture		= MAX(texture_t);
			material_t m_material	= MAX(material_t);
		};

	private:
		//- sprite atlas management
		detail::cspriteatlas_manager m_spriteatlas_manager;

		//- material management
		detail::cmaterial_manager m_material_manager;

		//- texture management
		detail::ctexture_manager m_texture_manager;

		//- shader program management
		detail::ctechnique_manager m_technique_manager;

		//- font management

		core::cmutex m_mutex;

		//- sprite draw commands
		vector_t< mat4_t > m_sprite_matrices;
		vector_t< core::srect > m_sprite_rects;
		vector_t< core::scolor > m_sprite_colors;
		map_t<unsigned, vector_t< ssprite_drawcommand > > m_draw_commands;

		//- camera for drawing current frame
		ref_t<ccamera> m_camera;

	private:
		bool load_default_and_embedded_shaders();
	};

} //- sm