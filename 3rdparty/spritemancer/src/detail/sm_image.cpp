#include "sm_image.hpp"

namespace sm
{
	namespace
	{
		//------------------------------------------------------------------------------------------------------------------------
		bimg::ImageContainer* image_allocate(texture_format format, unsigned w, unsigned h, unsigned depth,
			unsigned layers, bool mips, const void* data = nullptr)
		{
			return bimg::imageAlloc(entry::allocator(), bimg::TextureFormat::Enum(format),
				(uint16_t)w, (uint16_t)h, (uint16_t)depth, 1, false, mips, data);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void image_free(bimg::ImageContainer*& container)
		{
			bimg::imageFree(container); container = nullptr;
		}

	} //- unnamed

	namespace resource
	{
		//------------------------------------------------------------------------------------------------------------------------
		cimage::~cimage()
		{
			if (m_resource.m_image)
			{
				image_free(m_resource.m_image);
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cimage::create_from_solid_color(unsigned w, unsigned h, const core::scolor& color)
		{
			if (m_resource.m_image)
			{
				image_free(m_resource.m_image);
			}

			m_resource.m_image = image_allocate(texture_format::RGBA8, w, h, 1, 1, false);

			bimg::imageSolid(m_resource.m_image, w, h, color.rgba());
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cimage::create_checkerboard(unsigned w, unsigned h, unsigned step, const core::scolor& first, const core::scolor& second)
		{
			if (m_resource.m_image)
			{
				image_free(m_resource.m_image);
			}

			m_resource.m_image = image_allocate(texture_format::RGBA8, w, h, 1, 1, false);

			bimg::imageCheckerboard(m_resource.m_image, w, h, step, first.rgba(), second.rgba());
		}

		//------------------------------------------------------------------------------------------------------------------------
		const core::resource::iresource* cimage_manager::load(stringview_t name, const fs::cfileinfo& path)
		{
			unsigned size = 0;
			byte_t* data = nullptr;

			if (path.exists())
			{
				if (data = fs::load_binary_file_data(path.path(), &size); data && size > 0)
				{
					cimage image;

					//- Proceed loading from memory
					if (image.m_resource.m_image = bimg::imageParse(entry::allocator(), data, size); image.m_resource.m_image)
					{
						if (serror_reporter::instance().m_callback)
						{
							serror_reporter::instance().m_callback(core::logging_verbosity_trace,
								fmt::format("Successfully loaded image '{}' at '{}'", name, path.path()));
						}

						return emplace_object(name, std::move(image));
					}
				}
			}

			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					fmt::format("Failed loading image '{}' at '{}'", name, path.path()));
			}

			return nullptr;
		}

	} //- resource

} //- sm

RTTR_REGISTRATION
{
	using namespace sm::resource;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::registration::class_<cimage_manager>("cimage_manager")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
	;
}