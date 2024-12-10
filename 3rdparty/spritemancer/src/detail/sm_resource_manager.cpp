#include "sm_resource_manager.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	cimage_manager::cimage_manager(unsigned reserve /*= C_IMAGE_RESOURCE_MANAGER_RESERVE_COUNT*/)
	{
		m_data.reserve(reserve);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cimage_manager::~cimage_manager()
	{
		destroy_all(m_data);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cimage_manager::on_start()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cimage_manager::on_shutdown()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cimage_manager::on_update(float)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::image_handle_t cimage_manager::load_sync(stringview_t name, stringview_t filepath)
	{
		return load_of_sync<image_handle_t>(name, m_data, filepath);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::image_handle_t cimage_manager::load_sync(stringview_t name, void* data, unsigned size)
	{
		return load_of_sync<image_handle_t>(name, m_data, data, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::image_handle_t> cimage_manager::load_async(stringview_t name, stringview_t filepath)
	{
		return load_of_async<image_handle_t>(name, m_data, filepath);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::image_handle_t> cimage_manager::load_async(stringview_t name, void* data, unsigned size)
	{
		return load_of_async<image_handle_t>(name, m_data, data, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture_manager::ctexture_manager(unsigned reserve /*= C_TEXTURE_RESOURCE_MANAGER_RESERVE_COUNT*/)
	{
		m_data.reserve(reserve);
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture_manager::~ctexture_manager()
	{
		destroy_all(m_data);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool ctexture_manager::on_start()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ctexture_manager::on_shutdown()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ctexture_manager::on_update(float)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::texture_handle_t ctexture_manager::load_sync(stringview_t name, const cimage& image)
	{
		return load_of_sync<texture_handle_t>(name, m_data, image);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::texture_handle_t ctexture_manager::load_sync(stringview_t name, stringview_t filepath)
	{
		return load_of_sync<texture_handle_t>(name, m_data, filepath);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::texture_handle_t ctexture_manager::load_sync(stringview_t name, void* data, unsigned size, unsigned w, unsigned h, unsigned depth,
		bool mips, unsigned layers, texture_format format, uint64_t flags)
	{
		return load_of_sync<texture_handle_t>(name, m_data, data, size, w, h, depth, mips, layers, format, flags);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::texture_handle_t> ctexture_manager::load_async(stringview_t name, const cimage& image)
	{
		return load_of_async<texture_handle_t>(name, m_data, image);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::texture_handle_t> ctexture_manager::load_async(stringview_t name, stringview_t filepath)
	{
		return load_of_async<texture_handle_t>(name, m_data, filepath);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::texture_handle_t> ctexture_manager::load_async(stringview_t name, void* data, unsigned size, unsigned w, unsigned h, unsigned depth,
		bool mips, unsigned layers, texture_format format, uint64_t flags)
	{
		return load_of_async<texture_handle_t>(name, m_data, data, size, w, h, depth, mips, layers, format, flags);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader_manager::cshader_manager(unsigned reserve /*= C_SHADER_RESOURCE_MANAGER_RESERVE_COUNT*/)
	{
		m_data.reserve(reserve);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader_manager::~cshader_manager()
	{
		destroy_all(m_data);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cshader_manager::on_start()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cshader_manager::on_shutdown()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cshader_manager::on_update(float)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::shader_handle_t cshader_manager::load_sync(stringview_t name, stringview_t path)
	{
		return load_of_sync<shader_handle_t>(name, m_data, path);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::shader_handle_t cshader_manager::load_sync(stringview_t name, const char* text)
	{
		return load_of_sync<shader_handle_t>(name, m_data, text);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::shader_handle_t cshader_manager::load_sync(stringview_t name, const uint8_t* data, unsigned size)
	{
		return load_of_sync<shader_handle_t>(name, m_data, data, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::shader_handle_t> cshader_manager::load_async(stringview_t name, stringview_t path)
	{
		return load_of_async<shader_handle_t>(name, m_data, path);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::shader_handle_t> cshader_manager::load_async(stringview_t name, const char* text)
	{
		return load_of_async<shader_handle_t>(name, m_data, text);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::shader_handle_t> cshader_manager::load_async(stringview_t name, const uint8_t* data, unsigned size)
	{
		return load_of_async<shader_handle_t>(name, m_data, data, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cprogram_manager::cprogram_manager(unsigned reserve /*= C_PROGRAM_RESOURCE_MANAGER_RESERVE_COUNT*/)
	{
		m_data.reserve(reserve);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cprogram_manager::~cprogram_manager()
	{
		destroy_all(m_data);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cprogram_manager::on_start()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cprogram_manager::on_shutdown()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cprogram_manager::on_update(float)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::program_handle_t cprogram_manager::load_sync(stringview_t name, const shader_handle_t vs, const shader_handle_t fs)
	{
		return load_of_sync<program_handle_t>(name, m_data, vs, fs);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::program_handle_t cprogram_manager::load_sync(stringview_t name, const cshader& vs, const cshader& fs)
	{
		return load_of_sync<program_handle_t>(name, m_data, vs, fs);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::program_handle_t> cprogram_manager::load_async(stringview_t name, const shader_handle_t vs, const shader_handle_t fs)
	{
		return load_of_async<program_handle_t>(name, m_data, vs, fs);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::program_handle_t> cprogram_manager::load_async(stringview_t name, const cshader& vs, const cshader& fs)
	{
		return load_of_async<program_handle_t>(name, m_data, vs, fs);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cspriteatlas_manager::cspriteatlas_manager(unsigned reserve /*= C_SPRITEATLAS_RESOURCE_MANAGER_RESERVE_COUNT*/)
	{
		m_data.reserve(reserve);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cspriteatlas_manager::~cspriteatlas_manager()
	{
		destroy_all(m_data);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cspriteatlas_manager::on_start()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cspriteatlas_manager::on_shutdown()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cspriteatlas_manager::on_update(float)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::spriteatlas_handle_t cspriteatlas_manager::load_sync(stringview_t name,
		unsigned w, unsigned h, const vector_t<string_t>& names, const vec2_t& frames)
	{
		return load_of_sync<spriteatlas_handle_t>(name, m_data, w, h, names, frames);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::spriteatlas_handle_t cspriteatlas_manager::load_sync(stringview_t name, const cspriteatlas& other)
	{
		return load_of_sync<spriteatlas_handle_t>(name, m_data, other);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::spriteatlas_handle_t> cspriteatlas_manager::load_async(stringview_t name,
		unsigned w, unsigned h, const vector_t<string_t>& names, const vec2_t& frames)
	{
		return load_of_async<spriteatlas_handle_t>(name, m_data, w, h, names, frames);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::spriteatlas_handle_t> cspriteatlas_manager::load_async(stringview_t name, const cspriteatlas& other)
	{
		return load_of_async<spriteatlas_handle_t>(name, m_data, other);
	}

	//------------------------------------------------------------------------------------------------------------------------
	crendertarget_manager::crendertarget_manager(unsigned reserve /*= C_RENDERTARGET_RESOURCE_MANAGER_RESERVE_COUNT*/)
	{
		m_data.reserve(reserve);
	}

	//------------------------------------------------------------------------------------------------------------------------
	crendertarget_manager::~crendertarget_manager()
	{
		destroy_all(m_data);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool crendertarget_manager::on_start()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crendertarget_manager::on_shutdown()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crendertarget_manager::on_update(float)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::rendertarget_handle_t crendertarget_manager::load_sync(stringview_t name, unsigned w, unsigned h)
	{
		return load_of_sync<rendertarget_handle_t>(name, m_data, w, h);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::rendertarget_handle_t> crendertarget_manager::load_async(stringview_t name, unsigned w, unsigned h)
	{
		return load_of_async<rendertarget_handle_t>(name, m_data, w, h);
	}

} //- sm

RTTR_REGISTRATION
{
	using namespace sm;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::registration::class_<crendertarget_manager>("crendertarget_manager")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::registration::class_<cspriteatlas_manager>("cspriteatlas_manager")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::registration::class_<cshader_manager>("cshader_manager")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::registration::class_<ctexture_manager>("ctexture_manager")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::registration::class_<cimage_manager>("cimage_manager")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::registration::class_<cprogram_manager>("cprogram_manager")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
			)
		;
}