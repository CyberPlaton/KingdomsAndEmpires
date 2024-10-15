#pragma once
#include "detail/sm_config.hpp"

namespace sm
{
	namespace detail
	{
		uint64_t texture_gpu_memory_usage(int w, int h, int mips, texture_format format);
		uint64_t texture_bpp(texture_format format);

	} //- detail


	//- GPU resident image representation
	//------------------------------------------------------------------------------------------------------------------------
	class ctexture final : public core::cresource
	{
	public:
		static void destroy(ctexture& texture);

		explicit ctexture(const cimage& image);
		explicit ctexture(stringview_t filepath);
		explicit ctexture(image_type type, void* data, unsigned size);
		ctexture();
		~ctexture();

		opresult load_from_image(const cimage& image);
		opresult load_from_file(stringview_t filepath);
		opresult load_from_memory(image_type type, void* data, unsigned size);

		inline unsigned w() const { return m_texture.width; }
		inline unsigned h() const { return m_texture.height; }
		inline raylib::Texture2D texture() const { return m_texture; }

	private:
		raylib::Texture2D m_texture;

		RTTR_ENABLE(core::cresource);
	};

} //- sm