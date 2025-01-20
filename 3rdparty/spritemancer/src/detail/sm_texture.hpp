#pragma once
#include "sm_config.hpp"

namespace sm
{
	namespace resource
	{
		namespace detail
		{
			//- Struct containing GPU resident image data.
			//------------------------------------------------------------------------------------------------------------------------
			struct stexture final
			{
				bgfx::TextureInfo m_info;
				bgfx::TextureHandle m_handle;
			};

		} //- detail

		//- Texture resource class.
		//------------------------------------------------------------------------------------------------------------------------
		class ctexture final : public core::cresource<detail::stexture>
		{
		public:
			ctexture() = default;
			~ctexture();

			inline unsigned w() const { return m_resource.m_info.width; }
			inline unsigned h() const { return m_resource.m_info.height; }

			inline const auto& info() const { return m_resource.m_info; }
			inline const auto handle() const { return m_resource.m_handle; }

		private:
			RTTR_ENABLE(core::cresource<detail::stexture>);
		};

		//- Texture resource manager class responsible for loading a texture from file.
		//------------------------------------------------------------------------------------------------------------------------
		class ctexture_manager final : public core::cresource_manager<ctexture>
		{
		public:
			ctexture_manager() = default;
			~ctexture_manager() = default;

		protected:
			const core::resource::iresource* load(stringview_t name, const fs::cfileinfo& path) override final;

		private:
			RTTR_ENABLE(core::cresource_manager<ctexture>);
		};

	} //- resource

} //- sm