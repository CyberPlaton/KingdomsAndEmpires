#pragma once
#include "sm_config.hpp"

namespace sm
{
	namespace resource
	{
		namespace detail
		{
			//- Struct containing image data resident on CPU.
			//------------------------------------------------------------------------------------------------------------------------
			struct simage final
			{
				bimg::ImageContainer* m_image = nullptr;
			};

		} //- detail

		//- Image resource class.
		//------------------------------------------------------------------------------------------------------------------------
		class cimage final : public core::cresource<detail::simage>
		{
		public:
			cimage() = default;
			~cimage();

			void create_from_solid_color(unsigned w, unsigned h, const core::scolor& color);
			void create_checkerboard(unsigned w, unsigned h, unsigned step, const core::scolor& first, const core::scolor& second);

		private:
			RTTR_ENABLE(core::cresource<detail::simage>);
		};

		//- Image resource manager class responsible for loading an image from file.
		//------------------------------------------------------------------------------------------------------------------------
		class cimage_manager final : public core::cresource_manager<cimage>
		{
		public:
			cimage_manager() = default;
			~cimage_manager() = default;

		protected:
			const core::resource::iresource* load(stringview_t name, const fs::cfileinfo& path) override final;

		private:
			RTTR_ENABLE(core::cresource_manager<cimage>);
		};

	} //- resource

} //- sm