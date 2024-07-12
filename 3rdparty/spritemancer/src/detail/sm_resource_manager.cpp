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
		return load_of_sync<cimage, image_handle_t>(name, m_data, filepath);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::image_handle_t cimage_manager::load_sync(stringview_t name, image_type type, void* data, unsigned size)
	{
		return load_of_sync<cimage, image_handle_t>(name, m_data, type, data, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::image_handle_t> cimage_manager::load_async(stringview_t name, stringview_t filepath)
	{
		return load_of_async<cimage, image_handle_t>(name, m_data, filepath);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::image_handle_t> cimage_manager::load_async(stringview_t name, image_type type, void* data, unsigned size)
	{
		return load_of_async<cimage, image_handle_t>(name, m_data, type, data, size);
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
		return load_of_sync<ctexture, texture_handle_t>(name, m_data, image);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::texture_handle_t ctexture_manager::load_sync(stringview_t name, stringview_t filepath)
	{
		return load_of_sync<ctexture, texture_handle_t>(name, m_data, filepath);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::texture_handle_t ctexture_manager::load_sync(stringview_t name, image_type type, void* data, unsigned size)
	{
		return load_of_sync<ctexture, texture_handle_t>(name, m_data, type, data, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::texture_handle_t> ctexture_manager::load_async(stringview_t name, const cimage& image)
	{
		return load_of_async<ctexture, texture_handle_t>(name, m_data, image);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::texture_handle_t> ctexture_manager::load_async(stringview_t name, stringview_t filepath)
	{
		return load_of_async<ctexture, texture_handle_t>(name, m_data, filepath);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::texture_handle_t> ctexture_manager::load_async(stringview_t name, image_type type, void* data, unsigned size)
	{
		return load_of_async<ctexture, texture_handle_t>(name, m_data, type, data, size);
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
	sm::shader_handle_t cshader_manager::load_sync(stringview_t name, shader_type type,
		stringview_t vertex_filepath, stringview_t fragment_filepath)
	{
		return load_of_sync<cshader, shader_handle_t>(name, m_data, type, vertex_filepath, fragment_filepath);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::shader_handle_t cshader_manager::load_sync(stringview_t name, shader_type type, const char* vs, const char* fs)
	{
		return load_of_sync<cshader, shader_handle_t>(name, m_data, type, vs, fs);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::shader_handle_t cshader_manager::load_sync(stringview_t name, shader_type type,
		const uint8_t* vs, unsigned vs_size, const uint8_t* fs, unsigned fs_size)
	{
		return load_of_sync<cshader, shader_handle_t>(name, m_data, type, vs, vs_size, fs, fs_size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::shader_handle_t> cshader_manager::load_async(stringview_t name, shader_type type,
		stringview_t vertex_filepath, stringview_t fragment_filepath)
	{
		return load_of_async<cshader, shader_handle_t>(name, m_data, type, vertex_filepath, fragment_filepath);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::shader_handle_t> cshader_manager::load_async(stringview_t name, shader_type type,
		const char* vs, const char* fs)
	{
		return load_of_async<cshader, shader_handle_t>(name, m_data, type, vs, fs);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::shader_handle_t> cshader_manager::load_async(stringview_t name, shader_type type,
		const uint8_t* vs, unsigned vs_size, const uint8_t* fs, unsigned fs_size)
	{
		return load_of_async<cshader, shader_handle_t>(name, m_data, type, vs, vs_size, fs, fs_size);
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
		return load_of_sync<cspriteatlas, spriteatlas_handle_t>(name, m_data, w, h, names, frames);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::spriteatlas_handle_t> cspriteatlas_manager::load_async(stringview_t name,
		unsigned w, unsigned h, const vector_t<string_t>& names, const vec2_t& frames)
	{
		return load_of_async<cspriteatlas, spriteatlas_handle_t>(name, m_data, w, h, names, frames);
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
		return load_of_sync<crendertarget, rendertarget_handle_t>(name, m_data, w, h);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::rendertarget_handle_t> crendertarget_manager::load_async(stringview_t name, unsigned w, unsigned h)
	{
		return load_of_async<crendertarget, rendertarget_handle_t>(name, m_data, w, h);
	}

} //- sm