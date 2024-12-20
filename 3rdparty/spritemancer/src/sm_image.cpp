#include "sm_image.hpp"

namespace sm
{
	namespace
	{
		//------------------------------------------------------------------------------------------------------------------------
		image_type image_type_from_filepath(stringview_t filepath)
		{
			core::cpath path(filepath.data());

			const auto ext = path.extension();

			if (string_utils::compare(ext, extensions::C_PNG.data()))
			{
				return image_type_png;
			}
			else if (string_utils::compare(ext, extensions::C_BMP.data()))
			{
				return image_type_bmp;
			}
			else if (string_utils::compare(ext, extensions::C_TGA.data()))
			{
				return image_type_tga;
			}
			else if (string_utils::compare(ext, extensions::C_JPG.data()))
			{
				return image_type_jpg;
			}
			else if (string_utils::compare(ext, extensions::C_GIF.data()))
			{
				return image_type_gif;
			}
			else if (string_utils::compare(ext, extensions::C_PIC.data()))
			{
				return image_type_pic;
			}
			else if (string_utils::compare(ext, extensions::C_PSD.data()))
			{
				return image_type_psd;
			}
			else if (string_utils::compare(ext, extensions::C_HDR.data()))
			{
				return image_type_hdr;
			}
			else if (string_utils::compare(ext, extensions::C_QOI.data()))
			{
				return image_type_qoi;
			}
			else if (string_utils::compare(ext, extensions::C_SVG.data()))
			{
				return image_type_svg;
			}
			else if (string_utils::compare(ext, extensions::C_DDS.data()))
			{
				return image_type_dds;
			}
			else if (string_utils::compare(ext, extensions::C_PKM.data()))
			{
				return image_type_pkm;
			}
			else if (string_utils::compare(ext, extensions::C_KTX.data()))
			{
				return image_type_ktx;
			}
			else if (string_utils::compare(ext, extensions::C_PVR.data()))
			{
				return image_type_pvr;
			}
			else if (string_utils::compare(ext, extensions::C_ASTC.data()))
			{
				return image_type_astc;
			}

			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_warn, fmt::format("Unknown image extension '{}'!", ext));
			}

			return image_type_none;
		}

		//------------------------------------------------------------------------------------------------------------------------
		const char* image_file_type(image_type type)
		{
			switch (type)
			{
			case image_type_png:
			{
#ifdef SUPPORT_FILEFORMAT_PNG
				return extensions::C_PNG.data();
#endif
				break;
			}
			case image_type_bmp:
			{
#ifdef SUPPORT_FILEFORMAT_BMP
				return extensions::C_BMP.data();
#endif
				break;
			}
			case image_type_tga:
			{
#ifdef SUPPORT_FILEFORMAT_TGA
				return extensions::C_TGA.data();
#endif
				break;
			}
			case image_type_jpg:
			{
#ifdef SUPPORT_FILEFORMAT_JPG
				return extensions::C_JPG.data();
#endif
				break;
			}
			case image_type_gif:
			{
#ifdef SUPPORT_FILEFORMAT_GIF
				return extensions::C_GIF.data();
#endif
				break;
			}
			case image_type_pic:
			{
#ifdef SUPPORT_FILEFORMAT_PIC
				return extensions::C_PIC.data();
#endif
				break;
			}
			case image_type_psd:
			{
#ifdef SUPPORT_FILEFORMAT_PSD
				return extensions::C_PSD.data();
#endif
				break;
			}
			case image_type_hdr:
			{
#ifdef SUPPORT_FILEFORMAT_HDR
				return extensions::C_HDR.data();
#endif
				break;
			}
			case image_type_qoi:
			{
#ifdef SUPPORT_FILEFORMAT_QOI
				return extensions::C_QOI.data();
#endif
				break;
			}
			case image_type_svg:
			{
#ifdef SUPPORT_FILEFORMAT_SVG
				return extensions::C_SVG.data();
#endif
				break;
			}
			case image_type_dds:
			{
#ifdef SUPPORT_FILEFORMAT_DDS
				return extensions::C_DDS.data();
#endif
				break;
			}
			case image_type_pkm:
			{
#ifdef SUPPORT_FILEFORMAT_PKM
				return extensions::C_PKM.data();
#endif
				break;
			}
			case image_type_ktx:
			{
#ifdef SUPPORT_FILEFORMAT_KTX
				return extensions::C_KTX.data();
#endif
				break;
			}
			case image_type_pvr:
			{
#ifdef SUPPORT_FILEFORMAT_PVR
				return extensions::C_PVR.data();
#endif
				break;
			}
			case image_type_astc:
			{
#ifdef SUPPORT_FILEFORMAT_ASTC
				return extensions::C_ASTC.data();
#endif
				break;
			}
			default:
				break;
			}

			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_warn, fmt::format("Unknown image type '{}'!", algorithm::enum_to_string(type)));
			}

			return nullptr;
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	void cimage::destroy(cimage& image)
	{
		if (is_valid(image))
		{
			raylib::UnloadImage(image.image());
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	cimage::cimage(stringview_t filepath)
	{
		load_from_file(filepath);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cimage::cimage(image_type type, void* data, unsigned size)
	{
		load_from_memory(type, data, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cimage::cimage() :
		m_container({ 0 })
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cimage::cimage(image_generate_function_t&& callback) :
		m_container(callback())
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cimage::~cimage()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cimage::load_from_file(stringview_t filepath)
	{
		const auto memory = fs::load_binary_from_file(filepath.data());

		return load_from_memory(image_type_from_filepath(filepath), (void*)memory->data(), (unsigned)memory->size());
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cimage::load_from_memory(image_type type, void* data, unsigned size)
	{
		if (const char* ext = image_file_type(type); ext)
		{
			if (m_container = raylib::LoadImageFromMemory(ext, (const uint8_t*)data, size); raylib::IsImageReady(m_container))
			{
				return opresult_ok;
			}
		}

		if (serror_reporter::instance().m_callback)
		{
			serror_reporter::instance().m_callback(core::logging_verbosity_error, "Failed loading image");
		}

		return opresult_fail;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cimage::create_solid(unsigned w, unsigned h, const core::scolor& color)
	{
		if (raylib::IsImageReady(m_container))
		{
			raylib::UnloadImage(m_container);
		}

		m_container = raylib::GenImageColor(w, h, to_cliteral(color));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cimage::create_checkerboard(unsigned w, unsigned h, unsigned step,
		const core::scolor& first, const core::scolor& second)
	{
		if (raylib::IsImageReady(m_container))
		{
			raylib::UnloadImage(m_container);
		}

		m_container = raylib::GenImageChecked(w, h, w / step, h / step, to_cliteral(first), to_cliteral(second));
	}

} //- sm