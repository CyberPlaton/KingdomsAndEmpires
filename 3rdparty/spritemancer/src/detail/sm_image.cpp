#include "sm_image.hpp"

namespace sm
{
	namespace
	{
		//------------------------------------------------------------------------------------------------------------------------
		bimg::ImageContainer* allocate_image(texture_format format, unsigned w, unsigned h, unsigned depth,
			unsigned layers, bool mips, const void* data = nullptr)
		{
			return bimg::imageAlloc(entry::allocator(), bimg::TextureFormat::Enum(format),
				(uint16_t)w, (uint16_t)h, (uint16_t)depth, 1, false, mips, data);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void free_image(bimg::ImageContainer*& container)
		{
			bimg::imageFree(container); container = nullptr;
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	void cimage::destroy(cimage& image)
	{
		free_image(image.m_container);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cimage::cimage(stringview_t filepath)
	{
		load_from_file(filepath);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cimage::cimage(void* data, unsigned size)
	{
		load_from_memory(data, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cimage::cimage() :
		m_container(nullptr)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cimage::~cimage()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cimage::load_from_file(stringview_t filepath)
	{
		unsigned size = 0;
		const auto data = fs::load_binary_file_data(filepath.data(), &size);

		return load_from_memory(data, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cimage::load_from_memory(void* data, unsigned size)
	{
		if (m_container = bimg::imageParse(entry::allocator(), data, size); !m_container)
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Failed loading image");
			}

			return opresult_fail;
		}
		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cimage::create_solid(unsigned w, unsigned h, const core::scolor& color)
	{
		if (m_container)
		{
			free_image(m_container);
		}

		m_container = allocate_image(texture_format::RGBA8, w, h, 1, 1, false);

		bimg::imageSolid(m_container->m_data, w, h, color.rgba());
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cimage::create_checkerboard(unsigned w, unsigned h, unsigned step,
		const core::scolor& first, const core::scolor& second)
	{
		if (m_container)
		{
			free_image(m_container);
		}

		m_container = allocate_image(texture_format::RGBA8, w, h, 1, 1, false);

		bimg::imageCheckerboard(m_container->m_data, w, h, step, first.rgba(), second.rgba());
	}

	//------------------------------------------------------------------------------------------------------------------------
	cimage_manager::cimage_manager(unsigned reserve /*= C_IMAGE_RESOURCE_MANAGER_RESERVE_COUNT*/)
	{
		m_data.reserve(reserve);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cimage_manager::~cimage_manager()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cimage_manager::on_start()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cimage_manager::on_shutdown()
	{
		clear();
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

} //- sm

RTTR_REGISTRATION
{
	using namespace sm;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::registration::class_<cimage_manager>("cimage_manager")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
	;
}