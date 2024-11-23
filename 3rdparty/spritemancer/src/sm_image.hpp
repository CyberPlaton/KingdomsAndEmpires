#pragma once
#include "detail/sm_config.hpp"

namespace sm
{
	namespace extensions
	{
		constexpr stringview_t C_PNG = ".png";
		constexpr stringview_t C_BMP = ".bmp";
		constexpr stringview_t C_TGA = ".tga";
		constexpr stringview_t C_JPG = ".jpg";
		constexpr stringview_t C_GIF = ".gif";
		constexpr stringview_t C_PIC = ".pic";
		constexpr stringview_t C_PSD = ".psd";
		constexpr stringview_t C_HDR = ".hdr";
		constexpr stringview_t C_QOI = ".qoi";
		constexpr stringview_t C_SVG = ".svg";
		constexpr stringview_t C_DDS = ".dds";
		constexpr stringview_t C_PKM = ".pkm";
		constexpr stringview_t C_KTX = ".ktx";
		constexpr stringview_t C_PVR = ".pvr";
		constexpr stringview_t C_ASTC = ".astc";

	} //- extensions

	//- CPU resident image representation
	//------------------------------------------------------------------------------------------------------------------------
	class cimage final : public core::cresource
	{
	public:
		using image_generate_function_t = std::function<raylib::Image()>;

		static void destroy(cimage& image);

		explicit cimage(image_generate_function_t&& callback);
		explicit cimage(stringview_t filepath);
		explicit cimage(image_type type, void* data, unsigned size);
		cimage();
		~cimage();

		opresult load_from_file(stringview_t filepath);
		opresult load_from_memory(image_type type, void* data, unsigned size);

		inline raylib::Image image() const { return m_container; }

		//- utility functions for image generation and manipulation
		void create_solid(unsigned w, unsigned h, const core::scolor& color);
		void create_checkerboard(unsigned w, unsigned h, unsigned step, const core::scolor& first, const core::scolor& second);

	private:
		raylib::Image m_container;

		RTTR_ENABLE(core::cresource);
	};

} //- sm