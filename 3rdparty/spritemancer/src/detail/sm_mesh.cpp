#include "sm_mesh.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	cmesh::cmesh()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmesh::cmesh(geometry_type geometry, primitive_type primitive)
	{
		create(geometry, primitive);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmesh::~cmesh()
	{
		buffer::destroy(&buffer());
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmesh::create(geometry_type geometry, primitive_type primitive)
	{
		opresult result = opresult_fail;

		switch (geometry)
		{
		default:
		case geometry_type_none:
		case geometry_type_static:
		case geometry_type_dynamic:
		{
			buffer().m_buffer_type = (buffer_type)geometry;
			buffer().m_primitive_type = primitive;
			result = buffer::create(&buffer());
			break;
		}
		}

		if (!buffer().m_vertices.empty() && result != opresult_ok)
		{
			log_warn(fmt::format("Failed creating mesh from data for geometry type '{}' and primitive type '{}'",
				algorithm::enum_to_string(geometry), algorithm::enum_to_string(primitive)));
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmesh::update()
	{
		buffer::update(&buffer());
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmesh::bind()
	{
		switch (buffer().m_buffer_type)
		{
		default:
		case buffer_type_none:
		case buffer_type_static:
		case buffer_type_dynamic:
		{
			break;
		}
		case buffer_type_transient:
		{
			buffer::create(&buffer());
			break;
		}
		}

		buffer::bind(&buffer());
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmesh_manager::cmesh_manager(unsigned reserve /*= C_MESH_RESOURCE_MANAGER_RESERVE_COUNT*/)
	{
		m_data.reserve(reserve);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmesh_manager::~cmesh_manager()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmesh_manager::on_start()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmesh_manager::on_shutdown()
	{
		clear();
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