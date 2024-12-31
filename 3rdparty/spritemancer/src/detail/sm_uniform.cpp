#include "sm_uniform.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	cuniform::cuniform(stringview_t name, uniform_type type)
	{
		create(name, type);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cuniform::cuniform() :
		m_handle(C_INVALID_HANDLE)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cuniform::cuniform(stringview_t name) :
		m_name(name.data()), m_handle(C_INVALID_HANDLE)
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
		m_handle = bgfx::createUniform(name.data(), detail::to_bgfx_uniform_type(type), 1).idx;

		return is_valid(*this) ? opresult_ok : opresult_fail;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cuniform::create(stringview_t name)
	{
		m_name = name.data();

		return opresult_ok;
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

	//------------------------------------------------------------------------------------------------------------------------
	cuniform_manager::cuniform_manager(unsigned reserve /*= C_UNIFORM_RESOURCE_MANAGER_RESERVE_COUNT*/)
	{
		m_data.reserve(reserve);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cuniform_manager::~cuniform_manager()
	{
		clear();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cuniform_manager::on_start()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cuniform_manager::on_shutdown()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cuniform_manager::on_update(float)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::uniform_handle_t cuniform_manager::load_sync(stringview_t name, stringview_t uniform_name, uniform_type type)
	{
		return load_of_sync<uniform_handle_t>(name, m_data, uniform_name, type);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::uniform_handle_t cuniform_manager::load_sync(stringview_t name, stringview_t uniform_name)
	{
		return load_of_sync<uniform_handle_t>(name, m_data, uniform_name);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::uniform_handle_t> cuniform_manager::load_async(stringview_t name, stringview_t uniform_name, uniform_type type)
	{
		return load_of_async<mesh_handle_t>(name, m_data, uniform_name, type);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::uniform_handle_t> cuniform_manager::load_async(stringview_t name, stringview_t uniform_name)
	{
		return load_of_async<mesh_handle_t>(name, m_data, uniform_name);
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
