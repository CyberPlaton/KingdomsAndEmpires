#pragma once
#include "sm_config.hpp"

namespace sm
{
	//- CPU resident image representation
	//------------------------------------------------------------------------------------------------------------------------
	class cimage final : public core::cresource
	{
	public:
		static void destroy(cimage& image);

		explicit cimage(stringview_t filepath);
		explicit cimage(void* data, unsigned size);
		cimage();
		~cimage();

		opresult load_from_file(stringview_t filepath);
		opresult load_from_memory(void* data, unsigned size);

		inline const bimg::ImageContainer* image() const { return m_container; }

		//- utility functions for image generation and manipulation
		void create_solid(unsigned w, unsigned h, const core::scolor& color);
		void create_checkerboard(unsigned w, unsigned h, unsigned step, const core::scolor& first, const core::scolor& second);

	private:
		bimg::ImageContainer* m_container;

		RTTR_ENABLE(core::cresource);
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cimage_manager final :
		public core::cservice,
		public core::cresource_manager<cimage>
	{
	public:
		cimage_manager(unsigned reserve = C_IMAGE_RESOURCE_MANAGER_RESERVE_COUNT);
		~cimage_manager();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float) override final;

		image_handle_t load_sync(stringview_t name, stringview_t filepath);
		image_handle_t load_sync(stringview_t name, void* data, unsigned size);

		core::cfuture_type<image_handle_t> load_async(stringview_t name, stringview_t filepath);
		core::cfuture_type<image_handle_t> load_async(stringview_t name, void* data, unsigned size);

	private:
		RTTR_ENABLE(core::cservice, core::cresource_manager<cimage>);
	};

} //- sm