#pragma once
#include <core.h>
#include "sm_common.hpp"

namespace sm
{
	//- Constant time access to textures by handle. Logarithmic time loading and unloading.
	//------------------------------------------------------------------------------------------------------------------------
	class ctexture_manager final : core::iresource_manager
	{
	public:
		ctexture_manager();
		~ctexture_manager();

		texture_t load(stringview_t name, stringview_t path);
		texture_t at(stringview_t name) const;
		void unload(texture_t handle);

		const ctexture& get(texture_t handle) const;
		ctexture& get(texture_t handle);

	private:
		core::detail::cdynamic_pool<ctexture> m_textures;
		umap_t<string_t, texture_t> m_handles;
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cshader_manager final : core::iresource_manager
	{
	public:
		cshader_manager();
		~cshader_manager();

		technique_t load(stringview_t name, stringview_t vspath, stringview_t fspath);
		technique_t load_from_memory(stringview_t name, const char* vs, const char* fs);
		void unload(technique_t handle);
		technique_t at(stringview_t name) const;
		const ctechnique& get(technique_t handle) const;
		ctechnique& get(technique_t handle);

	private:
		core::detail::cdynamic_pool<ctechnique> m_shaders;
		umap_t<string_t, technique_t> m_handles;
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cmaterial_manager final : core::iresource_manager
	{
	public:
		cmaterial_manager();
		~cmaterial_manager();

		material_t load(stringview_t name, technique_t technique, blending_mode mode = blending_mode_alpha,
			blending_equation equation = blending_equation_blend_color, blending_factor src = blending_factor_src_color,
			blending_factor dst = blending_factor_one_minus_src_alpha);
		material_t at(stringview_t name) const;
		void unload(material_t handle);

		const cmaterial& get(material_t handle) const;

	private:
		core::detail::cdynamic_pool<cmaterial> m_materials;
		umap_t<string_t, material_t> m_handles;
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cspriteatlas_manager final : core::iresource_manager
	{
	public:
		cspriteatlas_manager();
		~cspriteatlas_manager();

		spriteatlas_t load(stringview_t name, texture_t texture, const vec2_t& frames);
		void unload(material_t handle);

		const cspriteatlas& get(spriteatlas_t handle) const;

	private:
		core::detail::cdynamic_pool<cspriteatlas> m_spritesheets;
		umap_t<string_t, spriteatlas_t> m_handles;
	};

	//------------------------------------------------------------------------------------------------------------------------
	class crendertarget_manager final : core::iresource_manager
	{
	public:
		crendertarget_manager();
		~crendertarget_manager();

		rendertarget_t load(stringview_t name, unsigned width, unsigned height);
		void unload(rendertarget_t handle);

		const crendertarget& get(rendertarget_t handle) const;


	private:
		core::detail::cdynamic_pool<crendertarget> m_rendertargets;
		umap_t<string_t, spriteatlas_t> m_handles;
	};

} //- sm