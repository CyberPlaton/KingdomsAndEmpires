#include "sm_texture.hpp"
#include "sm_image.hpp"

namespace sm
{
	namespace resource
	{
		//------------------------------------------------------------------------------------------------------------------------
		ctexture::~ctexture()
		{

		}

		//------------------------------------------------------------------------------------------------------------------------
		const core::resource::iresource* ctexture_manager::load(stringview_t name, const fs::cfileinfo& path)
		{
			auto* im = core::cservice_manager::find<resource::cimage_manager>();

			if (const auto* image = im->load_sync(name, path); image)
			{
				const auto* image_info = image->m_resource.m_image;

				const auto* mem = bgfx::makeRef(image_info->m_data, image_info->m_size);

				if (const auto handle = bgfx::createTexture2D((uint16_t)image_info->m_width, (uint16_t)image_info->m_height,
					image_info->m_numMips > 1, image_info->m_numLayers, (bgfx::TextureFormat::Enum)image_info->m_format,
					BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE, mem); bgfx::isValid(handle))
				{
					bgfx::TextureInfo texture_info;

					bgfx::calcTextureSize(texture_info, (uint16_t)image_info->m_width, (uint16_t)image_info->m_height,
						image_info->m_depth, image_info->m_cubeMap, image_info->m_numMips > 1, image_info->m_numLayers, (bgfx::TextureFormat::Enum)image_info->m_format);

					ctexture texture;
					texture.m_resource.m_handle = handle;
					texture.m_resource.m_info = std::move(texture_info);

					if (serror_reporter::instance().m_callback)
					{
						serror_reporter::instance().m_callback(core::logging_verbosity_trace,
							fmt::format("Successfully loaded texture '{}' at '{}'", name, path.path()));
					}

					return emplace_object(name, std::move(texture));
				}
			}

			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					fmt::format("Failed loading texture '{}' at '{}'", name, path.path()));
			}

			return nullptr;
		}

	} //- resource

} //- sm

RTTR_REGISTRATION
{
	using namespace sm::resource;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::registration::class_<ctexture_manager>("ctexture_manager")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;

}
