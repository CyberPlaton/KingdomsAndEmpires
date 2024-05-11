#include "sm_config.hpp"

namespace sm
{
	namespace entry
	{
		//------------------------------------------------------------------------------------------------------------------------
		ifilereader* file_reader()
		{
			if (!sdata::instance().S_FILEREADER)
			{
				sdata::instance().S_FILEREADER = std::make_unique<cfilereader>();
			}
			return sdata::instance().S_FILEREADER.get();
		}

		//------------------------------------------------------------------------------------------------------------------------
		ifilewriter* file_writer()
		{
			if (!sdata::instance().S_FILEWRITER)
			{
				sdata::instance().S_FILEWRITER = std::make_unique<cfilewriter>();
			}
			return sdata::instance().S_FILEWRITER.get();
		}

		//------------------------------------------------------------------------------------------------------------------------
		bx::AllocatorI* allocator()
		{
			if (!sdata::instance().S_ALLOCATOR)
			{
				sdata::instance().S_ALLOCATOR = std::make_unique<bx::DefaultAllocator>();
			}
			return sdata::instance().S_ALLOCATOR.get();
		}

		//------------------------------------------------------------------------------------------------------------------------
		iimage_loader* image_loader()
		{
			if (!sdata::instance().S_IMAGE_LOADER)
			{
				sdata::instance().S_IMAGE_LOADER = std::make_unique<cimage_loader>();
			}
			return sdata::instance().S_IMAGE_LOADER.get();
		}

		//------------------------------------------------------------------------------------------------------------------------
		ishader_loader* shader_loader()
		{
			if (!sdata::instance().S_SHADER_LOADER)
			{
				sdata::instance().S_SHADER_LOADER = std::make_unique<cshader_loader>();
			}
			return sdata::instance().S_SHADER_LOADER.get();
		}

		//------------------------------------------------------------------------------------------------------------------------
		sm::irenderer* renderer()
		{
			if (!sdata::instance().S_RENDERER)
			{
				CORE_ASSERT(false, "Invalid operation. Renderer was not set before using it");
			}
			return sdata::instance().S_RENDERER.get();
		}

		//------------------------------------------------------------------------------------------------------------------------
		sm::iplatform* platform()
		{
			if (!sdata::instance().S_PLATFORM)
			{
				CORE_ASSERT(false, "Invalid operation. Platform was not set before using it");
			}
			return sdata::instance().S_PLATFORM.get();
		}

		//------------------------------------------------------------------------------------------------------------------------
		sm::iapp* app()
		{
			if (!sdata::instance().S_APP)
			{
				CORE_ASSERT(false, "Invalid operation. App was not set before using it");
			}
			return sdata::instance().S_APP;
		}

		//------------------------------------------------------------------------------------------------------------------------
		sm::opresult cimage_loader::from_file(bimg::ImageContainer*& container, stringview_t filepath,
			bimg::TextureFormat::Enum format)
		{
			const auto [data, size] = core::cfile::load_binary(filepath.data());

			bx::Error error;

			if (container = bimg::imageParse(allocator(), (void*)data, size, format, &error); !error.isOk() || !container)
			{
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(core::logging_verbosity_error,
						fmt::format("Failed to load image '{}'", filepath.data()));
				}
				return opresult_fail;
			}
			return opresult_ok;
		}

		//------------------------------------------------------------------------------------------------------------------------
		sm::opresult cimage_loader::create(bimg::ImageContainer*& container, unsigned w, unsigned h,
			const core::scolor& tint, bimg::TextureFormat::Enum format)
		{
			if (container)
			{
				free(container);
			}

			if (container = bimg::imageAlloc(entry::allocator(), format, w, h, 1, 1, false, false); container)
			{
				bimg::imageSolid(container->m_data, w, h, tint.rgba());

				return opresult_ok;
			}
			return opresult_fail;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cimage_loader::free(bimg::ImageContainer*& container)
		{
			bimg::imageFree(container); container = nullptr;
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool cfilereader::open(const bx::FilePath& path, bx::Error* error)
		{
			if (const auto result = ifilereader::open(path, error); !result)
			{
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(core::logging_verbosity_warn,
						fmt::format("Failed to read from file '{}'", path.getCPtr()));
				}
				return false;
			}
			return true;
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool cfilewriter::open(const bx::FilePath& path, bool append, bx::Error* error)
		{
			if (const auto result = ifilewriter::open(path, append, error); !result)
			{
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(core::logging_verbosity_warn,
						fmt::format("Failed to write to file '{}'", path.getCPtr()));
				}
				return false;
			}
			return true;
		}

	} //- entry

	//------------------------------------------------------------------------------------------------------------------------
	cimage::~cimage()
	{
		entry::image_loader()->free(m_container);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cimage::load_from_file(stringview_t filepath, bimg::TextureFormat::Enum format /*= bimg::TextureFormat::RGBA8*/)
	{
		return entry::image_loader()->from_file(m_container, filepath, format);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cimage::create(unsigned w, unsigned h, bimg::TextureFormat::Enum format /*= bimg::TextureFormat::RGBA8*/)
	{
		return entry::image_loader()->create(m_container, w, h, {255, 255, 255, 255}, format);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult crenderable::load(stringview_t filepath, bool filtering /*= false*/, bool clamped /*= false*/,
		bimg::TextureFormat::Enum format /*= bimg::TextureFormat::RGBA8*/)
	{
		if (const auto result = m_image.load_from_file(filepath, format); result == opresult_ok)
		{
			const auto w = m_image.m_container->m_width;
			const auto h = m_image.m_container->m_height;

			//- create GPU side
			m_texture.m_id = entry::renderer()->create_texture(w, h, filtering, clamped);

			//- update GPU side
			entry::renderer()->bind_texture(m_texture.m_id);
			entry::renderer()->update_texture(m_texture.m_id, w, h, m_image.m_container->m_data);
		}
		return opresult_fail;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderable::create(unsigned w, unsigned h, bool filtering /*= false*/, bool clamped /*= false*/,
		bimg::TextureFormat::Enum format /*= bimg::TextureFormat::RGBA8*/)
	{
		//- create CPU side first
		m_image.create(w, h, format);

		//- create GPU side
		m_texture.m_id = entry::renderer()->create_texture(w, h, filtering, clamped);

		//- update GPU side
		entry::renderer()->bind_texture(m_texture.m_id);
		entry::renderer()->update_texture(m_texture.m_id, w, h, m_image.m_container->m_data);
	}

} //- sm