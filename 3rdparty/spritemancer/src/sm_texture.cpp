#include "sm_texture.hpp"
#include "sm_image.hpp"

namespace sm
{
	namespace detail
	{
		//------------------------------------------------------------------------------------------------------------------------
		uint64_t texture_gpu_memory_usage(int w, int h, int mips, texture_format format)
		{
			uint64_t usage = 0;

			switch (format)
			{
			default:
				break;
			case texture_format_r8:
			{
				//- luminance, grayscale, i.e. generally one channel
				usage = static_cast<uint64_t>(w * h);
			}
			case texture_format_r8g8:
			case texture_format_r5g6b5:
			{
				//- two channels
				usage = static_cast<uint64_t>(w * h * 2);
			}
			case texture_format_r16g16b16a16:
			{
				//- RGBA16
				usage = static_cast<uint64_t>(w * h * 4 * 2);
			}
			case texture_format_r8g8b8:
			{
				//- RGB
				usage = static_cast<uint64_t>(w * h * 3);
			}
			case texture_format_qoi:
			case texture_format_r8g8b8a8:
			{
				//- RGBA
				usage = static_cast<uint64_t>(w * h * 4);
			}
			case texture_format_r32g32b32a32:
			{
				//- RGBA32
				usage = static_cast<uint64_t>(w * h * 4 * 4);
			}
			case texture_format_bc1:
			{
				//- RGB and binary color, 8:1 compression ratio
				usage = (w * h * 4) / 8;
			}
			case texture_format_bc3:
			case texture_format_bc2:
			{
				//- RGB and gradient alpha, 4:1 compression ratio
				usage = (w * h * 4) / 4;
			}
			//- @reference: https://www.cs.cmu.edu/afs/cs/academic/class/15869-f11/www/readings/nystad12_astc.pdf
			case texture_format_astc_4x4:
			{
				//- 8bpp
				usage = w * h;
			}
			case texture_format_astc_8x8:
			{
				//- 2bpp
				usage = (w * h) / 4;
			}
			}

			return usage;
		}

		//------------------------------------------------------------------------------------------------------------------------
		uint64_t texture_bpp(texture_format format)
		{
			uint64_t bpp = 0;

			switch (format)
			{
			default:
				break;
			case texture_format_r8:
			{
				//- luminance, grayscale, i.e. generally one channel
				bpp = 8;
			}
			case texture_format_r8g8:
			case texture_format_r5g6b5:
			{
				//- two channels
				bpp = 16;
			}
			case texture_format_r16g16b16a16:
			{
				//- RGBA16
				bpp = (uint64_t)16 * 4;
			}
			case texture_format_r8g8b8:
			{
				//- RGB
				bpp = 24;
			}
			case texture_format_qoi:
			case texture_format_r8g8b8a8:
			{
				//- RGBA
				bpp = 32;
			}
			case texture_format_r32g32b32a32:
			{
				//- RGBA32
				bpp = (uint64_t)32 * 4;
			}
			case texture_format_bc1:
			{
				//- RGB and binary color, 8:1 compression ratio
				bpp = 4;
			}
			case texture_format_bc3:
			case texture_format_bc2:
			{
				//- RGB and gradient alpha, 4:1 compression ratio
				bpp = 8;
			}
			//- @reference: https://www.cs.cmu.edu/afs/cs/academic/class/15869-f11/www/readings/nystad12_astc.pdf
			case texture_format_astc_4x4:
			{
				//- 8bpp
				bpp = 8;
			}
			case texture_format_astc_8x8:
			{
				//- 2bpp
				bpp = 2;
			}
			}

			return bpp;
		}

	} //- detail

	//------------------------------------------------------------------------------------------------------------------------
	void ctexture::destroy(ctexture& texture)
	{
		if (is_valid(texture))
		{
			raylib::UnloadTexture(texture.texture());

			texture.m_texture = { 0 };
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture::ctexture() :
		m_texture({ 0 })
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
	ctexture::ctexture(image_type type, void* data, unsigned size)
	{
		load_from_memory(type, data, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture::~ctexture()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult ctexture::load_from_image(const cimage& image)
	{
		if (m_texture = raylib::LoadTextureFromImage(image.image()); !raylib::IsTextureReady(m_texture))
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Failed loading texture");
			}

			return opresult_fail;
		}

		return opresult_ok;
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
	sm::opresult ctexture::load_from_memory(image_type type, void* data, unsigned size)
	{
		cimage image(type, data, size);

		if (is_valid(image) && load_from_image(image))
		{
			return opresult_ok;
		}

		return opresult_fail;
	}

} //- sm