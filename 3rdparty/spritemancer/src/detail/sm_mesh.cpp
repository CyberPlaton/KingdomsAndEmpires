#include "sm_mesh.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	cmesh::cmesh(rttr::type vertex_type) :
		m_buffer(vertex_type)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmesh::cmesh()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmesh::bind()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cmesh::create(rttr::type vertex_type)
	{
		m_buffer.set_vertex_type(vertex_type);

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cmesh::create()
	{
		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmesh_manager::cmesh_manager(unsigned reserve /*= C_MESH_RESOURCE_MANAGER_RESERVE_COUNT*/)
	{
		m_data.reserve(reserve);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmesh_manager::~cmesh_manager()
	{
		destroy_all(m_data);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmesh_manager::on_start()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmesh_manager::on_shutdown()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmesh_manager::on_update(float)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	mesh_handle_t cmesh_manager::load_sync(stringview_t name, rttr::type vertex_type)
	{
		return load_of_sync<mesh_handle_t>(name, m_data, vertex_type);
	}

	//------------------------------------------------------------------------------------------------------------------------
	mesh_handle_t cmesh_manager::load_sync(stringview_t name)
	{
		return load_of_sync<mesh_handle_t>(name, m_data);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<mesh_handle_t> cmesh_manager::load_async(stringview_t name, rttr::type vertex_type)
	{
		return load_of_async<mesh_handle_t>(name, m_data, vertex_type);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<mesh_handle_t> cmesh_manager::load_async(stringview_t name)
	{
		return load_of_async<mesh_handle_t>(name, m_data);
	}

} //- sm

RTTR_REGISTRATION
{
	using namespace sm;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::registration::class_<cmesh_manager>("cmesh_manager")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;

}