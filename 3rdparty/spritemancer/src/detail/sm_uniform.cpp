#include "sm_uniform.hpp"

namespace sm
{
	namespace uniform
	{

	} //- uniform

	//------------------------------------------------------------------------------------------------------------------------
	void cuniform::destroy(cuniform& uniform)
	{
		if (is_valid(uniform))
		{
			bgfx::destroy(bgfx::UniformHandle{ uniform.uniform() });
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	cuniform::cuniform(stringview_t name, bgfx::UniformType::Enum type)
	{
		create(name, type);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cuniform::cuniform() :
		m_handle(MAX(handle_type_t))
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cuniform::~cuniform()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	opresult cuniform::create(stringview_t name, bgfx::UniformType::Enum type)
	{
		m_handle = bgfx::createUniform(name.data(), type, 1).idx;

		return bgfx::isValid(bgfx::UniformHandle{ m_handle }) ? opresult_ok : opresult_fail;
	}

	//------------------------------------------------------------------------------------------------------------------------
	uniform_handle_t cuniform::uniform() const
	{
		return m_handle;
	}

	//------------------------------------------------------------------------------------------------------------------------
	const void* cuniform::value() const
	{
		return m_value.data().get_raw_ptr();
	}

} //- sm

RTTR_REGISTRATION
{
	using namespace sm;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::registration::class_<cuniform_manager>("cuniform_manager")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}
