#include "sm_uniform.hpp"

namespace sm
{
	namespace
	{
        //------------------------------------------------------------------------------------------------------------------------
        bgfx::UniformType::Enum to_bgfx_enum(uniform_type type)
        {
            switch(type)
            {
                case uniform_type_float:
                case uniform_type_vector2:
                case uniform_type_vector3:
                case uniform_type_vector4:
                    return bgfx::UniformType::Enum::Vec4;
                case uniform_type_mat3x3:
                    return bgfx::UniformType::Enum::Mat3;
                case uniform_type_mat4x4:
                    return bgfx::UniformType::Enum::Mat4;
                case uniform_type_sampler:
                    return bgfx::UniformType::Enum::Sampler;
                default:
                    return bgfx::UniformType::Enum::Count;
            }
        }
    
	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	cuniform::cuniform(stringview_t name, uniform_type type)
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
        if (is_valid(*this))
        {
            bgfx::destroy(*this);
        }
	}

	//------------------------------------------------------------------------------------------------------------------------
	opresult cuniform::create(stringview_t name, uniform_type type)
	{
		m_handle = bgfx::createUniform(name.data(), to_bgfx_enum(type), 1).idx;

		return is_valid(*this) ? opresult_ok : opresult_fail;
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
