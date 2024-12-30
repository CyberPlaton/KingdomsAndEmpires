#include "sm_config.hpp"

namespace sm
{
	struct sbuffer;

	//------------------------------------------------------------------------------------------------------------------------
	enum buffer_type : uint8_t
	{
		buffer_type_none = 0,
		buffer_type_static,
		buffer_type_dynamic,
		buffer_type_transient,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum primitive_type : uint8_t
	{
		primitive_type_none = 0,
		primitive_type_triangles,
		primitive_type_lines,
		primitive_type_points,
	};

	namespace buffer
	{
		opresult create(sbuffer* buffer);
		void destroy(sbuffer* buffer);
		void update(sbuffer* buffer);
		void bind(sbuffer* buffer);

	} //- buffer

	//- Buffer containing vertex and index data.
	//------------------------------------------------------------------------------------------------------------------------
	struct sbuffer final
	{
		vector_t<float> m_vertices;
		vector_t<index_type_t> m_indices;

		stringview_t m_vertex_type_name;
		buffer_type m_buffer_type = buffer_type_none;
		primitive_type m_primitive_type = primitive_type_none;
		buffer_handle_t m_vbh = C_INVALID_HANDLE;
		buffer_handle_t m_ibh = C_INVALID_HANDLE;
	};

} //- sm