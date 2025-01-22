#include "sm_material.hpp"
#include "sm_texture.hpp"
#include "sm_program.hpp"

namespace sm
{
	namespace resource
	{
		namespace
		{
		} //- unnamed

		//------------------------------------------------------------------------------------------------------------------------
		void cmaterial::bind(unsigned depth /*= 0*/)
		{
			CORE_ASSERT(bgfx::isValid(m_resource.m_texture_data.m_handle) &&
				bgfx::isValid(m_resource.m_program_data.m_handle), "Invalid operation. Binding an invalid material with an invalid texture or program!");

			bgfx::setTexture(0, m_resource.m_sampler_data.m_handle, m_resource.m_texture_data.m_handle,
				m_resource.m_sampler_data.m_sampler_flags);

			for (const auto& uniform : m_resource.m_uniforms)
			{
				bgfx::setUniform(uniform.m_handle, uniform.m_data.data().get_raw_ptr(), 1u);
			}

			bgfx::setState(m_resource.m_state);
			bgfx::submit(m_resource.m_view, m_resource.m_program_data.m_handle, depth);
		}

	} //- resource

} //- sm

RTTR_REGISTRATION
{
	using namespace sm::resource;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::cregistrator<detail::smaterial::ssampler_data>("smaterial::ssampler_data")
		.prop("m_uniform_name", &detail::smaterial::ssampler_data::m_uniform_name)
		.prop("m_sampler_flags", &detail::smaterial::ssampler_data::m_sampler_flags)
		;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::cregistrator<detail::smaterial::stexture_data>("smaterial::stexture_data")
		.prop("m_texture_path", &detail::smaterial::stexture_data::m_texture_path)
		;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::cregistrator<detail::smaterial::sprogram_data>("smaterial::sprogram_data")
		.prop("m_program_path", &detail::smaterial::sprogram_data::m_program_path)
		;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::cregistrator<detail::smaterial::suniform_data>("smaterial::suniform_data")
		.prop("m_uniform_name", &detail::smaterial::suniform_data::m_uniform_name)
		;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::cregistrator<detail::smaterial>("detail::smaterial")
		.prop("m_uniforms", &detail::smaterial::m_uniforms)
		.prop("m_sampler_data", &detail::smaterial::m_sampler_data)
		.prop("m_program_data", &detail::smaterial::m_program_data)
		.prop("m_texture_data", &detail::smaterial::m_texture_data)
		.prop("m_state", &detail::smaterial::m_state)
		;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::registration::class_<cmaterial_manager>("cmaterial_manager")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}