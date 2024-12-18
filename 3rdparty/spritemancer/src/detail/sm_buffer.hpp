#include "sm_config.hpp"

//- Macro for defining common functions for a vertex type structure. After the class name we expect the parameters to
//- make a vertex, which reflects the vertex types members.
//------------------------------------------------------------------------------------------------------------------------
#define DECLARE_VERTEX_TYPE(c) \
	inline static bgfx::VertexLayout S_LAYOUT; \
	inline static bgfx::VertexLayoutHandle S_HANDLE; \
	static void init_buffer(core::cany& b) \
	{ \
		b = core::cany(vector_t<c>()); \
	} \
	static vertex_layout_handle_t vertex_layout_handle() \
	{ \
		return S_HANDLE.idx; \
	} \
	static bgfx::VertexLayout vertex_layout() \
	{ \
		return S_LAYOUT; \
	} \
	static stringview_t name() { static constexpr stringview_t C_NAME = SSTRING(c); return C_NAME; } \
	static unsigned vertex_count(core::cany& b) { return SCAST(unsigned, b.get<vector_t<c>>().size()); } \
	static bool vertex_empty(core::cany& b) { return b.get<vector_t<c>>().empty(); } \
	static void vertex_clear(core::cany& b) { b.get<vector_t<c>>().clear(); } \
	static void vertex_destroy(core::cany& b, sm::buffer_handle_t handle, sm::detail::buffer_type type) \
	{ \
		if (bgfx::isValid(bgfx::VertexBufferHandle{ handle })) \
		{ \
			switch (type) \
			{ \
			case sm::detail::buffer_type_static: \
			{ \
				bgfx::destroy(bgfx::VertexBufferHandle{ handle }); \
				break; \
			} \
			case sm::detail::buffer_type_dynamic: \
			{ \
				bgfx::destroy(bgfx::DynamicVertexBufferHandle{ handle }); \
				break; \
			} \
			default: \
			case sm::detail::buffer_type_transient: \
			{ \
				break; \
			} \
			} \
		} \
	} \
	static void vertex_swap_buffers(core::cany& b, core::cany& front) \
	{ \
		auto& b_data = b.get<vector_t<c>>(); \
		auto& front_data = front.get<vector_t<c>>(); \
		stl::swap(b_data, front_data); \
	} \
	static void vertex_buffer_create(core::cany& b, sm::buffer_handle_t& handle, sm::detail::buffer_type type) \
	{ \
		auto& vertices = b.get<vector_t<c>>(); \
		switch (type) \
		{ \
		case sm::detail::buffer_type_static: \
		{ \
			if (!vertices.empty()) \
			{ \
				handle = bgfx::createVertexBuffer(bgfx::makeRef(vertices.data(), vertices.size()), \
					c::vertex_layout()).idx; \
			} \
			break; \
		} \
		case sm::detail::buffer_type_dynamic: \
		{ \
			if (!vertices.empty()) \
			{ \
				handle = bgfx::createDynamicVertexBuffer(bgfx::makeRef(vertices.data(), vertices.size()), \
					c::vertex_layout(), BGFX_BUFFER_ALLOW_RESIZE).idx; \
			} \
			break; \\
		} \
		default: \
		case sm::detail::buffer_type_transient: \
		{ \
			break; \
		} \
		} \
	} \
	static void vertex_buffer_update(core::cany& b, sm::buffer_handle_t handle, sm::detail::buffer_type type, int index) \
	{ \
		auto& vertices = b.get<vector_t<c>>(); \
		switch(type) \
		{ \
		case sm::detail::buffer_type_static: break; \
		case sm::detail::buffer_type_dynamic: bgfx::update(bgfx::DynamicVertexBufferHandle{ handle }, index, bgfx::makeRef(vertices.data(), vertices.size()));\
		default: \
		case sm::detail::buffer_type_transient: break; \
		} \
	} \
static void vertex_push(core::cany& b, const core::cany& v) \
{ \
	auto& vertices = b.get<vector_t<c>>(); \
	vertices.emplace_back(v.get<c>()); \
}

//- Use macro to reflect you vertex type to RTTR.
//------------------------------------------------------------------------------------------------------------------------
#define REGISTER_VERTEX_TYPE(c) \
	rttr::cregistrator<c>(SSTRING(c)) \
		.meth(sm::detail::C_VERTEX_TYPE_INIT_FUNC_NAME, &c::init) \
		.meth(sm::detail::C_VERTEX_TYPE_NAME_FUNC_NAME, &c::name) \
		.meth(sm::detail::C_VERTEX_TYPE_INIT_BUFFER_FUNC_NAME, &c::init_buffer) \
		.meth(sm::detail::C_VERTEX_TYPE_MAKE_VERTEX_FUNC_NAME, &c::make_vertex) \
		.meth(sm::detail::C_VERTEX_TYPE_VERTEX_LAYOUT_FUNC_NAME, &c::vertex_layout) \
		.meth(sm::detail::C_VERTEX_TYPE_VERTEX_COUNT_FUNC_NAME, &c::vertex_count) \
		.meth(sm::detail::C_VERTEX_TYPE_VERTEX_CLEAR_FUNC_NAME, &c::vertex_clear) \
		.meth(sm::detail::C_VERTEX_TYPE_VERTEX_DESTROY_FUNC_NAME, &c::vertex_destroy) \
		.meth(sm::detail::C_VERTEX_TYPE_VERTEX_EMPTY_FUNC_NAME, &c::vertex_empty) \
		.meth(sm::detail::C_VERTEX_TYPE_VERTEX_SWAP_BUFFERS_FUNC_NAME, &c::vertex_swap_buffers) \
		.meth(sm::detail::C_VERTEX_TYPE_VERTEX_BUFFER_CREATE_FUNC_NAME, &c::vertex_buffer_create) \
		.meth(sm::detail::C_VERTEX_TYPE_VERTEX_BUFFER_UPDATE_FUNC_NAME, &c::vertex_buffer_update) \
		.meth(sm::detail::C_VERTEX_TYPE_VERTEX_PUSH_FUNC_NAME, &c::vertex_push) \
		.meth(sm::detail::C_VERTEX_TYPE_VERTEX_LAYOUT_HANDLE_FUNC_NAME, &c::vertex_layout_handle)

namespace sm
{
	namespace detail
	{
		constexpr rttr::string_view C_VERTEX_TYPE_INIT_FUNC_NAME = "init";
		constexpr rttr::string_view C_VERTEX_TYPE_NAME_FUNC_NAME = "name";
		constexpr rttr::string_view C_VERTEX_TYPE_INIT_BUFFER_FUNC_NAME = "init_buffer";
		constexpr rttr::string_view C_VERTEX_TYPE_MAKE_VERTEX_FUNC_NAME = "make_vertex";
		constexpr rttr::string_view C_VERTEX_TYPE_VERTEX_LAYOUT_FUNC_NAME = "vertex_layout";
		constexpr rttr::string_view C_VERTEX_TYPE_VERTEX_LAYOUT_HANDLE_FUNC_NAME = "vertex_layout_handle";
		constexpr rttr::string_view C_VERTEX_TYPE_VERTEX_COUNT_FUNC_NAME = "vertex_count";
		constexpr rttr::string_view C_VERTEX_TYPE_VERTEX_CLEAR_FUNC_NAME = "vertex_clear";
		constexpr rttr::string_view C_VERTEX_TYPE_VERTEX_DESTROY_FUNC_NAME = "vertex_destroy";
		constexpr rttr::string_view C_VERTEX_TYPE_VERTEX_EMPTY_FUNC_NAME = "vertex_empty";
		constexpr rttr::string_view C_VERTEX_TYPE_VERTEX_PUSH_FUNC_NAME = "vertex_push";
		constexpr rttr::string_view C_VERTEX_TYPE_VERTEX_SWAP_BUFFERS_FUNC_NAME = "vertex_swap_buffers";
		constexpr rttr::string_view C_VERTEX_TYPE_VERTEX_BUFFER_CREATE_FUNC_NAME = "vertex_buffer_create";
		constexpr rttr::string_view C_VERTEX_TYPE_VERTEX_BUFFER_UPDATE_FUNC_NAME = "vertex_buffer_update";

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

		//- Base class for all vertex types. See 'sposition_color_texcoord_vertex' as example on how to define one.
		//------------------------------------------------------------------------------------------------------------------------
		struct ivertex_type
		{
			static stringview_t name() { static constexpr stringview_t C_NAME = "ivertex_type"; return C_NAME; }

			RTTR_ENABLE();
		};

		//------------------------------------------------------------------------------------------------------------------------
		class cbuffer final
		{
		public:
			cbuffer(rttr::type vertex_type);
			~cbuffer() = default;

			cbuffer& push_vertex(const core::cany& vertex);
			cbuffer& push_index(index_type_t index);

			cbuffer& push_line(const core::cany& vertex);
			cbuffer& push_point(const core::cany& vertex);
			cbuffer& push_line(const core::cany& vertex1, const core::cany& vertex2);
			cbuffer& push_triangle(const core::cany& vertex1, const core::cany& vertex2, const core::cany& vertex3);
			cbuffer& push_quad(const core::cany& vertex1, const core::cany& vertex2, const core::cany& vertex3, const core::cany& vertex4);

			unsigned vertices_count() const;
			unsigned indices_count() const;
			bool vertices_empty() const;
			bool indices_empty() const;
			bool valid() const;

			void submit();
			void swap_buffers();
			void clear();
			void destroy();

			inline void set_buffer_type(buffer_type type) { m_buffer_type = type; }
			inline buffer_type get_buffer_type() const { return m_buffer_type; }

			inline void set_primitive_type(primitive_type type) { m_primitive_type = type; }
			inline primitive_type get_primitive_type() const { return m_primitive_type; }

			inline buffer_handle_t vertex_buffer() const { return m_vertex_buffer_handle; }
			inline buffer_handle_t index_buffer() const { return m_index_buffer_handle; }

		private:
			core::cany m_vertex_data;
			core::cany m_vertex_data_front;
			vector_t<index_type_t> m_indices;
			rttr::type m_vertex_type;
			buffer_type m_buffer_type = buffer_type_none;
			primitive_type m_primitive_type = primitive_type_none;
			buffer_handle_t m_vertex_buffer_handle = MAX(buffer_handle_t);
			buffer_handle_t m_index_buffer_handle = MAX(buffer_handle_t);

		private:
			void create_buffers();
			void push_last_index();
			void push_current_index();
		};

	} //- detail

} //- sm