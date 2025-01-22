#include "sm_material.hpp"
#include "sm_texture.hpp"
#include "sm_program.hpp"

namespace sm
{
	namespace resource
	{
		//------------------------------------------------------------------------------------------------------------------------
		void cmaterial::bind(unsigned depth /*= 0*/)
		{
			CORE_ASSERT(bgfx::isValid(m_resource.m_texture_data.m_handle) &&
				bgfx::isValid(m_resource.m_program_data.m_handle), "Invalid operation. Binding an invalid material with an invalid texture or program!");

			bgfx::setTexture(0, m_resource.m_sampler_data.m_handle, m_resource.m_texture_data.m_handle,
				m_resource.m_sampler_data.m_sampler_flags);

			for (const auto& uniform : m_resource.m_uniforms)
			{
				bgfx::setUniform(uniform.m_handle, uniform.m_data, 1u);
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
	rttr::registration::class_<cmaterial_manager>("cmaterial_manager")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}