#include "sm_material.hpp"
#include "sm_texture.hpp"
#include "sm_program.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	cmaterial::cmaterial(stringview_t program, stringview_t texture) :
		m_program(C_INVALID_HANDLE), m_texture(C_INVALID_HANDLE), m_owning_program(false), m_owning_texture(false)
	{
		create(program, texture);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmaterial::cmaterial() :
		m_program(C_INVALID_HANDLE), m_texture(C_INVALID_HANDLE), m_owning_program(false), m_owning_texture(false)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	cmaterial::~cmaterial()
	{
		m_uniforms.clear();

		if (m_owning_program &&
			m_program != C_INVALID_HANDLE)
		{
			auto* pm = core::cservice_manager::find<cprogram_manager>(); pm->erase(m_program);
		}
		if (m_owning_texture &&
			m_texture != C_INVALID_HANDLE)
		{
			auto* tm = core::cservice_manager::find<ctexture_manager>(); tm->erase(m_texture);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cmaterial::create(stringview_t program, stringview_t texture)
	{
		auto* pm = core::cservice_manager::find<cprogram_manager>();
		auto* tm = core::cservice_manager::find<ctexture_manager>();

		m_program = pm->handle(program);
		m_texture = tm->handle(texture);

		if (!pm->lookup(program) ||
			!tm->lookup(texture))
		{
			log_warn(fmt::format("Loading material without having program '{}' or texture '{}' loaded. This might lead to a crash!",
				program.data(), texture.data()));
		}

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial::bind(uint8_t view /*= 0*/, unsigned depth /*= 0*/)
	{
		CORE_ASSERT(is_valid(*this), "Invalid operation. Binding an invalid material with an invalid texture or program!");

		bgfx::setTexture(0, m_sampler_state.m_sampler, bgfx::TextureHandle{ m_texture }, m_sampler_state.m_sampler_flags);

		for (const auto& uniform : m_uniforms)
		{
			bgfx::setUniform(uniform, uniform.value(), 1u);
		}

		bgfx::setState(m_state);
		bgfx::submit(view, bgfx::ProgramHandle{ m_program }, depth);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmaterial_manager::cmaterial_manager(unsigned reserve /*= C_MATERIAL_RESOURCE_MANAGER_RESERVE_COUNT*/)
	{
		m_data.reserve(reserve);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmaterial_manager::~cmaterial_manager()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmaterial_manager::on_start()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial_manager::on_shutdown()
	{
		clear();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial_manager::on_update(float)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::material_handle_t cmaterial_manager::load_sync(stringview_t name, stringview_t program, stringview_t texture)
	{
		return load_of_sync<image_handle_t>(name, m_data, program, texture);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::material_handle_t cmaterial_manager::load_sync(stringview_t name)
	{
		return load_of_sync<image_handle_t>(name, m_data);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::material_handle_t> cmaterial_manager::load_async(stringview_t name, stringview_t program, stringview_t texture)
	{
		return load_of_async<image_handle_t>(name, m_data, program, texture);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::material_handle_t> cmaterial_manager::load_async(stringview_t name)
	{
		return load_of_async<image_handle_t>(name, m_data);
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