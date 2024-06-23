#pragma once
#include "sm_config.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	class cimage_manager final :
		public core::cservice,
		public core::iresource_manager<cimage, image_handle_t>
	{
	public:
		cimage_manager(unsigned reserve = C_IMAGE_RESOURCE_MANAGER_RESERVE_COUNT);
		~cimage_manager();

		image_handle_t load_sync(stringview_t name, stringview_t filepath);
		image_handle_t load_sync(stringview_t name, image_type type, void* data, unsigned size);

		core::cfuture_type<image_handle_t> load_async(stringview_t name, stringview_t filepath);
		core::cfuture_type<image_handle_t> load_async(stringview_t name, image_type type, void* data, unsigned size);

	private:
		RTTR_ENABLE(core::cservice);
	};

	//------------------------------------------------------------------------------------------------------------------------
	class ctexture_manager final :
		public core::cservice,
		public core::iresource_manager<ctexture, texture_handle_t>
	{
	public:
		ctexture_manager(unsigned reserve = C_TEXTURE_RESOURCE_MANAGER_RESERVE_COUNT);
		~ctexture_manager();

		texture_handle_t load_sync(stringview_t name, const cimage& image);
		texture_handle_t load_sync(stringview_t name, stringview_t filepath);
		texture_handle_t load_sync(stringview_t name, image_type type, void* data, unsigned size);

		core::cfuture_type<texture_handle_t> load_async(stringview_t name, const cimage& image);
		core::cfuture_type<texture_handle_t> load_async(stringview_t name, stringview_t filepath);
		core::cfuture_type<texture_handle_t> load_async(stringview_t name, image_type type, void* data, unsigned size);

	private:
		RTTR_ENABLE(core::cservice);
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cshader_manager final :
		public core::cservice,
		public core::iresource_manager<cshader, shader_handle_t>
	{
	public:
		cshader_manager(unsigned reserve = C_SHADER_RESOURCE_MANAGER_RESERVE_COUNT);
		~cshader_manager();

		shader_handle_t load_sync(stringview_t name, shader_type type, stringview_t vertex_filepath, stringview_t fragment_filepath);
		shader_handle_t load_sync(stringview_t name, shader_type type, const char* vs, const char* fs);
		shader_handle_t load_sync(stringview_t name, shader_type type, const uint8_t* vs, unsigned vs_size, const uint8_t* fs, unsigned fs_size);

		core::cfuture_type<shader_handle_t> load_async(stringview_t name, shader_type type, stringview_t vertex_filepath, stringview_t fragment_filepath);
		core::cfuture_type<shader_handle_t> load_async(stringview_t name, shader_type type, const char* vs, const char* fs);
		core::cfuture_type<shader_handle_t> load_async(stringview_t name, shader_type type, const uint8_t* vs, unsigned vs_size, const uint8_t* fs, unsigned fs_size);

	private:
		RTTR_ENABLE(core::cservice);
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cspriteatlas_manager final :
		public core::cservice,
		public core::iresource_manager<cspriteatlas, spriteatlas_handle_t>
	{
	public:
		cspriteatlas_manager(unsigned reserve = C_SPRITEATLAS_RESOURCE_MANAGER_RESERVE_COUNT);
		~cspriteatlas_manager();

		spriteatlas_handle_t load_sync(stringview_t name, unsigned w, unsigned h, const vector_t<string_t>& names, const vec2_t& frames);

		core::cfuture_type<spriteatlas_handle_t> load_async(stringview_t name, unsigned w, unsigned h, const vector_t<string_t>& names, const vec2_t& frames);

	private:
		RTTR_ENABLE(core::cservice);
	};

	//------------------------------------------------------------------------------------------------------------------------
	class crendertarget_manager final :
		public core::cservice,
		public core::iresource_manager<crendertarget, rendertarget_handle_t>
	{
	public:
		crendertarget_manager(unsigned reserve = C_RENDERTARGET_RESOURCE_MANAGER_RESERVE_COUNT);
		~crendertarget_manager();

		rendertarget_handle_t load_sync(stringview_t name, unsigned w, unsigned h);

		core::cfuture_type<rendertarget_handle_t> load_async(stringview_t name, unsigned w, unsigned h);

	private:
		RTTR_ENABLE(core::cservice);
	};

} //- sm

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(cimage_manager)
	{
		rttr::registration::class_<cimage_manager>("cimage_manager")
			.constructor<>()
			(
				rttr::policy::ctor::as_raw_ptr
			)
			;
	}

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(ctexture_manager)
	{
		rttr::registration::class_<ctexture_manager>("ctexture_manager")
			.constructor<>()
			(
				rttr::policy::ctor::as_raw_ptr
			)
			;
	}

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(cshader_manager)
	{
		rttr::registration::class_<cshader_manager>("cshader_manager")
			.constructor<>()
			(
				rttr::policy::ctor::as_raw_ptr
			)
			;
	}

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(cspriteatlas_manager)
	{
		rttr::registration::class_<cspriteatlas_manager>("cspriteatlas_manager")
			.constructor<>()
			(
				rttr::policy::ctor::as_raw_ptr
			)
			;
	}

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(crendertarget_manager)
	{
		rttr::registration::class_<crendertarget_manager>("crendertarget_manager")
			.constructor<>()
			(
				rttr::policy::ctor::as_raw_ptr
			)
			;
	}
} //- sm