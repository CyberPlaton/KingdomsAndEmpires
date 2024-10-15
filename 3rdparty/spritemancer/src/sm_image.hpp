#pragma once
#include "detail/sm_config.hpp"

namespace sm
{
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