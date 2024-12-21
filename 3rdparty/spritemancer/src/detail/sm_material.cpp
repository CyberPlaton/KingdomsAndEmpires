#include "sm_material.hpp"
#include "sm_resource_manager.hpp"

namespace sm
{
	namespace material
	{
		//------------------------------------------------------------------------------------------------------------------------
		sm::opresult create(smaterial* material, stringview_t name, stringview_t program, stringview_t texture)
		{
			auto* mm = core::cservice_manager::find<cmaterial_manager>();

			if (auto future = mm->load_async(name, program, texture); future.ready())
			{
				auto& mat = mm->get(future.get());


			}

			return opresult_fail;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void destroy(smaterial* material)
		{
			cprogram::destroy(material->m_program);
			ctexture::destroy(material->m_texture);
			for (auto& uniform : material->m_uniforms)
			{
				cuniform::destroy(uniform);
			}
			material->m_uniforms.clear();
		}

		//------------------------------------------------------------------------------------------------------------------------
		void bind(smaterial* material, uint8_t view /*= 0*/, unsigned depth /*= 0*/)
		{
			bgfx::setTexture(0, material->m_sampler, material->m_texture, material->m_sampler_flags);

			for (const auto& uniform : material->m_uniforms)
			{
				bgfx::setUniform(uniform, uniform.value(), 1u);
			}

			bgfx::setState(material->m_state);
			bgfx::submit(view, material->m_program, depth);
		}

	} //- material

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial::destroy(cmaterial& material)
	{
		auto* pm = core::cservice_manager::find<cprogram_manager>();
		auto* tm = core::cservice_manager::find<ctexture_manager>();
		auto* um = core::cservice_manager::find<cuniform_manager>();


		pm->release(material.m_program);
		tm->release(material.m_texture);

		algorithm::for_each(material.m_uniforms.begin(), material.m_uniforms.end(),
			[=](auto& uniform)
				{
					um->release(uniform);
				});

		cprogram::destroy(material.m_program);
		ctexture::destroy(material->m_texture);
		for (auto& uniform : material->m_uniforms)
		{
			cuniform::destroy(uniform);
		}
		material->m_uniforms.clear();
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmaterial::cmaterial(stringview_t program, stringview_t texture)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cmaterial::create(stringview_t program, stringview_t texture)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial::bind(uint8_t view /*= 0*/, unsigned depth /*= 0*/)
	{
		bgfx::setTexture(0, m_sampler_state.m_sampler, bgfx::TextureHandle{ m_texture }, m_sampler_state.m_sampler_flags);

		for (const auto& uniform : m_uniforms)
		{
			bgfx::setUniform(uniform, uniform.value(), 1u);
		}

		bgfx::setState(m_state);
		bgfx::submit(view, bgfx::ProgramHandle{ m_program }, depth);
	}


} //- sm

RTTR_REGISTRATION
{
	using namespace sm;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::registration::class_<cmaterial_manager>("cmaterial_manager")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}