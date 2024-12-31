#include "sm_texture.hpp"
#include "sm_image.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	ctexture::ctexture() :
		m_texture(C_INVALID_HANDLE)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture::ctexture(const cimage& image)
	{
		load_from_image(image);
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture::ctexture(stringview_t filepath)
	{
		load_from_file(filepath);
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture::ctexture(void* data, unsigned size, unsigned w, unsigned h, unsigned depth,
		bool mips, unsigned layers, texture_format format, uint64_t flags)
	{
		load_from_memory(data, size, w, h, depth, mips, layers, format, flags);
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture::ctexture(texture_handle_t handle, const bgfx::TextureInfo& info) :
		m_texture(handle), m_info(info)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture::~ctexture()
	{
		if (bgfx::isValid(bgfx::TextureHandle{ texture() }))
		{
			bgfx::destroy(bgfx::TextureHandle{ texture() });

			//- reset handle and info
			m_texture = C_INVALID_HANDLE;
			m_info = {};
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult ctexture::load_from_image(const cimage& image)
	{
		const auto w = image.image()->m_width;
		const auto h = image.image()->m_height;
		const auto mips = image.image()->m_numMips < 1;
		const auto& layers = image.image()->m_numLayers;
		const auto format = image.image()->m_format;
		const auto& depth = image.image()->m_depth;

		return load_from_memory(image.image()->m_data, image.image()->m_size, w, h, depth, mips, layers,
			texture_format(format), BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult ctexture::load_from_file(stringview_t filepath)
	{
		cimage image(filepath);

		if (is_valid(image) && load_from_image(image))
		{
			return opresult_ok;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult ctexture::load_from_memory(void* data, unsigned size, unsigned w, unsigned h, unsigned depth,
		bool mips, unsigned layers, texture_format format, uint64_t flags)
	{
		cimage image(data, size);

		if (is_valid(image) && load_from_image(image))
		{
			return opresult_ok;
		}

		return opresult_fail;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture_manager::ctexture_manager(unsigned reserve /*= C_TEXTURE_RESOURCE_MANAGER_RESERVE_COUNT*/)
	{
		m_data.reserve(reserve);
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture_manager::~ctexture_manager()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool ctexture_manager::on_start()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ctexture_manager::on_shutdown()
	{
		clear();
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

} //- sm

RTTR_REGISTRATION
{
	using namespace sm;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::registration::class_<ctexture_manager>("ctexture_manager")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;

}
