#include "sm_config.hpp"

//- Macro for defining common functions for a vertex type structure. After the class name we expect the parameters to
//- make a vertex, which reflects the vertex types members.
//------------------------------------------------------------------------------------------------------------------------
#define DECLARE_VERTEX_TYPE(c) \
	static bgfx::VertexLayout S_LAYOUT; \
	static bgfx::VertexLayoutHandle S_HANDLE; \
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
	static unsigned vertices_count(core::cany& b) const { return SCAST(unsigned, b.get<vector_t<c>>().size()); } \
	static bool vertices_empty(core::cany& b) const { return b.get<vector_t<c>>().empty(); } \
static void vertices_clear(core::cany& b) { b.get<vector_t<c>>().clear(); } \
static void vertices_destroy(core::cany& b, buffer_handle_t handle, buffer_type type) \
{ \
	if (bgfx::isValid(bgfx::VertexBufferHandle{ handle })) \
	{ \
		switch (type) \
		{ \
		case buffer_type_static: \
		{ \
			bgfx::destroy(bgfx::VertexBufferHandle{ handle }); \
			break; \
		} \
		case buffer_type_dynamic: \
		{ \
			bgfx::destroy(bgfx::DynamicVertexBufferHandle{ handle }); \
			break; \
		} \
		case buffer_type_transient: \
		{ \
			break; \
		} \
		} \
	} \
} \

//- Use macro to reflect you vertex type to RTTR.
//------------------------------------------------------------------------------------------------------------------------
#define REGISTER_VERTEX_TYPE(c) \
	rttr::cregistrator<c>(SSTRING(c)) \
		.meth(sm::detail::C_VERTEX_TYPE_INIT_FUNC_NAME, &c::init) \
		.meth(sm::detail::C_VERTEX_TYPE_NAME_FUNC_NAME, &c::name) \
		.meth(sm::detail::C_VERTEX_TYPE_INIT_BUFFER_FUNC_NAME, &c::init_buffer) \
		.meth(sm::detail::C_VERTEX_TYPE_MAKE_VERTEX_FUNC_NAME, &c::make_vertex) \
		.meth(sm::detail::C_VERTEX_TYPE_VERTEX_LAYOUT_FUNC_NAME, &c::vertex_layout) \
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
		constexpr rttr::string_view C_VERTEX_TYPE_VERTICES_COUNT_FUNC_NAME = "vertex_count";
		constexpr rttr::string_view C_VERTEX_TYPE_VERTICES_CLEAR_FUNC_NAME = "vertex_clear";
		constexpr rttr::string_view C_VERTEX_TYPE_VERTICES_DESTROY_FUNC_NAME = "vertex_destroy";
		constexpr rttr::string_view C_VERTEX_TYPE_VERTICES_EMPTY_FUNC_NAME = "vertex_empty";

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
		class cbuffer2 final
		{
		public:
			cbuffer2(rttr::type vertex_type);
			~cbuffer2() = default;

			unsigned vertices_count() const;
			unsigned indices_count() const;
			bool vertices_empty() const;
			bool indices_empty() const;

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
			vector_t<index_type_t> m_indices;
			rttr::type m_vertex_type;
			buffer_type m_buffer_type = buffer_type_none;
			primitive_type m_primitive_type = primitive_type_none;
			buffer_handle_t m_vertex_buffer_handle = MAX(buffer_handle_t);
			buffer_handle_t m_index_buffer_handle = MAX(buffer_handle_t);
		};

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		class cbuffer final
		{
		public:
			cbuffer() = default;
			~cbuffer() { destroy(); }

			unsigned vertices_size() const { return SCAST(unsigned, m_vertex_buffer_front.size()); }
			unsigned indices_size() const { return SCAST(unsigned, m_index_buffer_front.size()); }
			bgfx::VertexLayout vertex_layout() const;

			void push_vertex(const TVertex& vertex);
			void push_index(index_type_t index);
			void push_point(const TVertex& vertex);
			void push_line(const TVertex& vertex);
			void push_line(const TVertex& vertex1, const TVertex& vertex2);
			void push_triangle(const TVertex& vertex1, const TVertex& vertex2, const TVertex& vertex3);
			void push_quad(const TVertex& vertex1, const TVertex& vertex2, const TVertex& vertex3, const TVertex& vertex4);

			void init();
			void clear();
			void destroy();
			bool is_valid();
			void reserve(unsigned vertices, unsigned indices);
			void submit();
			bool vertex_empty() const { return m_vertex_buffer.empty(); }
			bool index_empty() const { return m_index_buffer.empty(); }

			TVertex* vertex_data() const { return m_vertex_buffer.data(); }
			index_type_t* index_data() const { return m_index_buffer.data(); }

			void set_buffer_type(buffer_type type) { m_buffer_type = type; }
			buffer_type get_buffer_type() const { return m_buffer_type; }

			void set_primitive_type(primitive_type type) { m_primitive_type = type; }
			primitive_type get_primitive_type() const { return m_primitive_type; }

			buffer_handle_t vertex_buffer() const { return m_vertex_buffer_handle; }
			buffer_handle_t index_buffer() const { return m_index_buffer_handle; }

		private:
			vector_t<TVertex> m_vertex_buffer;
			vector_t<TVertex> m_vertex_buffer_front;

			vector_t<index_type_t> m_index_buffer;
			vector_t<index_type_t> m_index_buffer_front;

			buffer_type m_buffer_type = buffer_type_none;
			primitive_type m_primitive_type = primitive_type_none;
			buffer_handle_t m_vertex_buffer_handle = MAX(buffer_handle_t);
			buffer_handle_t m_index_buffer_handle = MAX(buffer_handle_t);

		private:
			void swap_buffers();

		};

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		bgfx::VertexLayout cbuffer<TVertex>::vertex_layout() const
		{
			return TVertex::s_vertex_layout;
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		void cbuffer<TVertex>::init()
		{
			switch (buffer_type())
			{
			case buffer_type_static:
			{
				if (!vertex_buffer_empty())
				{
					m_vertex_buffer_handle = bgfx::createVertexBuffer(bgfx::makeRef(vertex_buffer_data(), vertex_buffer_size()),
						vertex_layout()).idx;

					if (!index_buffer_empty())
					{
						m_index_buffer_handle = bgfx::createIndexBuffer(bgfx::makeRef(index_buffer_data(), index_buffer_size())).idx;
					}
				}
				break;
			}
			case buffer_type_dynamic:
			{
				if (!vertex_buffer_empty())
				{
					m_vertex_buffer_handle = bgfx::createDynamicVertexBuffer(bgfx::makeRef(vertex_buffer_data(), vertex_buffer_size()),
						vertex_layout(), BGFX_BUFFER_ALLOW_RESIZE).idx;

					if (!index_buffer_empty())
					{
						m_index_buffer_handle = bgfx::createDynamicIndexBuffer(bgfx::makeRef(index_buffer_data(), index_buffer_size()),
							BGFX_BUFFER_ALLOW_RESIZE).idx;
					}
				}
				break;
			}
			case buffer_type_transient:
			{
				break;
			}
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		void cbuffer<TVertex>::submit()
		{
			switch (buffer_type())
			{
			case buffer_type_static:
			{
				destroy();
				init();
				break;
			}
			case buffer_type_dynamic:
			{
				if (!valid())
				{
					init();
				}
				else
				{
					bgfx::update(bgfx::DynamicVertexBufferHandle{ vertex_buffer() }, index,
						bgfx::makeRef(vertex_buffer_data(), vertex_buffer_size()));

					bgfx::update(bgfx::DynamicIndexBufferHandle{ index_buffer() }, index,
						bgfx::makeRef(index_buffer_data(), index_buffer_size()));
				}
				break;
			}
			case buffer_type_transient:
			{
				break;
			}
			}
			swap_buffers();
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		void cbuffer<TVertex>::swap_buffers()
		{
			stl::swap(m_vertex_buffer, m_vertex_buffer_front);
			stl::swap(m_index_buffer, m_index_buffer_front);
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		bool cbuffer<TVertex>::is_valid()
		{
			return bgfx::isValid(bgfx::VertexBufferHandle{ vertex_buffer() }) &&
				bgfx::isValid(bgfx::IndexBufferHandle{ index_buffer() });
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		void cbuffer<TVertex>::destroy()
		{
			if (bgfx::isValid(bgfx::VertexBufferHandle{ vertex_buffer() }))
			{
				switch (buffer_type())
				{
				case buffer_type_static:
				{
					bgfx::destroy(bgfx::VertexBufferHandle{ vertex_buffer() });
					break;
				}
				case buffer_type_dynamic:
				{
					bgfx::destroy(bgfx::DynamicVertexBufferHandle{ vertex_buffer() });
					break;
				}
				case buffer_type_transient:
				{
					break;
				}
				}
			}

			if (bgfx::isValid(bgfx::IndexBufferHandle{ index_buffer() }))
			{
				switch (buffer_type())
				{
					case buffer_type_static:
					{
						bgfx::destroy(bgfx::IndexBufferHandle{ index_buffer() });
						break;
					}
					case buffer_type_dynamic:
					{
						bgfx::destroy(bgfx::DynamicIndexBufferHandle{ index_buffer() });
						break;
					}
					case buffer_type_transient:
					{
						break;
					}
				}
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		void cbuffer<TVertex>::reserve(unsigned vertices, unsigned indices)
		{
			m_vertex_buffer.reserve(m_vertex_buffer.size() + vertices);
			m_index_buffer.reserve(m_index_buffer.size() + indices);
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		void cbuffer<TVertex>::clear()
		{
			m_vertex_buffer.clear();
			m_index_buffer.clear();
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		void cbuffer<TVertex>::push_quad(const TVertex& vertex1, const TVertex& vertex2, const TVertex& vertex3, const TVertex& vertex4)
		{
			add_vertex(vertex1);
			add_vertex(vertex2);
			add_vertex(vertex3);
			add_vertex(vertex4);

			const auto i = SCAST(index_type_t, m_index_buffer.size());
			add_index(i);
			add_index(i + 1);
			add_index(i + 2);
			add_index(i + 3);
			add_index(i + 1);
			add_index(i + 3);
			add_index(i + 2);
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		void cbuffer<TVertex>::push_triangle(const TVertex& vertex1, const TVertex& vertex2, const TVertex& vertex3)
		{
			add_vertex(vertex1);
			add_vertex(vertex2);
			add_vertex(vertex3);

			const auto i = SCAST(index_type_t, m_index_buffer.size());
			add_index(i);
			add_index(i + 1);
			add_index(i + 2);
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		void cbuffer<TVertex>::push_line(const TVertex& vertex1, const TVertex& vertex2)
		{
			add_vertex(vertex1);
			add_last_vertex_index();

			add_vertex(vertex2);
			add_last_vertex_index();
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		void cbuffer<TVertex>::add_current_vertex_index()
		{
			add_index(static_cast<index_type_t>(m_index_buffer.size()));
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		void cbuffer<TVertex>::push_line(const TVertex& vertex)
		{
			if (m_vertex_buffer.empty())
			{
				add_last_vertex_index();
				add_current_vertex_index
			}

			add_vertex(vertex);
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		void cbuffer<TVertex>::push_point(const TVertex& vertex)
		{
			add_vertex(vertex);
			add_last_vertex_index();
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		void cbuffer<TVertex>::add_last_vertex_index()
		{
			add_index(static_cast<index_type_t>(m_index_buffer.size() - 1));
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		void cbuffer<TVertex>::push_index(index_type_t index)
		{
			m_index_buffer.emplace_back(index);
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		void cbuffer<TVertex>::push_vertex(const TVertex& vertex)
		{
			m_vertex_buffer.emplace_back(vertex);
		}

	} //- detail

} //- sm