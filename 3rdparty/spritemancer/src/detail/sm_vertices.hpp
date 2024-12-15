#include "sm_config.hpp"

namespace sm
{
	namespace detail
	{
		class ivertices;
		using vertices_ref_t = ref_t<ivertices>;

		//------------------------------------------------------------------------------------------------------------------------
		class ivertices
		{
		public:
			enum buffer_type : uint8_t
			{
				buffer_type_none = 0,
				buffer_type_static,
				buffer_type_dynamic,
				buffer_type_transient,
			};

			enum primitive_type : uint8_t
			{
				primitive_type_none = 0,
				primitive_type_triangles,
				primitive_type_lines,
				primitive_type_points,
			};

			explicit ivertices(rttr::type vertex_type);

			void set_buffer_type(buffer_type type) { m_buffer_type = type; }
			buffer_type get_buffer_type() const { return m_buffer_type; }

			void set_primitive_type(primitive_type type) { m_primitive_type = type; }
			primitive_type get_primitive_type() const { return m_primitive_type; }

			buffer_handle_t vertex_buffer() const { return m_vertex_buffer_handle; }
			buffer_handle_t index_buffer() const { return m_index_buffer_handle; }

		protected:
			buffer_type m_buffer_type = buffer_type_none;
			primitive_type m_primitive_type = primitive_type_none;
			buffer_handle_t m_vertex_buffer_handle = MAX(buffer_handle_t);
			buffer_handle_t m_index_buffer_handle = MAX(buffer_handle_t);
		};

		//------------------------------------------------------------------------------------------------------------------------
		class cvertices : public ivertices
		{
		public:
			explicit cvertices(rttr::type vertex_type);

			unsigned vertex_buffer_size() const override final;
			unsigned index_buffer_size() const override final;
			bgfx::VertexLayout vertex_layout() const override final;

			void add_point(const TVertex& vertex);
			void add_line(const TVertex& vertex);
			void add_line(const TVertex& vertex1, const TVertex& vertex2);
			void add_triangle(const TVertex& vertex1, const TVertex& vertex2, const TVertex& vertex3);
			void add_quad(const TVertex& vertex1, const TVertex& vertex2, const TVertex& vertex3, const TVertex& vertex4);

			void init();
			void clear();
			void destroy();
			bool is_valid();
			void reserve(unsigned vertices, unsigned indices);
			void submit();
			bool vertex_buffer_empty();
			bool index_buffer_empty();
			void swap_buffers();

			void add_vertex(const TVertex& vertex);
			void add_index(index_type_t index);

			void add_last_vertex_index();
			void add_current_vertex_index();

			TVertex* vertex_buffer_data() const { return m_vertex_buffer.data(); }
			index_type_t* index_buffer_data() const { return m_index_buffer.data(); }

		private:
			vector_t<TVertex> m_vertex_buffer;
			vector_t<TVertex> m_vertex_buffer_front;

			vector_t<index_type_t> m_index_buffer;
			vector_t<index_type_t> m_index_buffer_front;
		};

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		bool cmesh<TVertex>::index_buffer_empty()
		{
			return m_index_buffer.empty();
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		bool cmesh<TVertex>::vertex_buffer_empty()
		{
			return m_vertex_buffer.empty();
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		bgfx::VertexLayout cmesh<TVertex>::vertex_layout() const
		{
			return TVertex::s_vertex_layout;
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		void cmesh<TVertex>::init()
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
		void cmesh<TVertex>::submit()
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
		void cmesh<TVertex>::swap_buffers()
		{
			stl::swap(m_vertex_buffer, m_vertex_buffer_front);
			stl::swap(m_index_buffer, m_index_buffer_front);
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		bool cmesh<TVertex>::is_valid()
		{
			return bgfx::isValid(bgfx::VertexBufferHandle{ vertex_buffer() }) &&
				bgfx::isValid(bgfx::IndexBufferHandle{ index_buffer() });
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		void cmesh<TVertex>::destroy()
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
		void cmesh<TVertex>::reserve(unsigned vertices, unsigned indices)
		{
			m_vertex_buffer.reserve(m_vertex_buffer.size() + vertices);
			m_index_buffer.reserve(m_index_buffer.size() + indices);
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		void cmesh<TVertex>::clear()
		{
			m_vertex_buffer.clear();
			m_index_buffer.clear();
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		void cmesh<TVertex>::add_quad(const TVertex& vertex1, const TVertex& vertex2, const TVertex& vertex3, const TVertex& vertex4)
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
		void cmesh<TVertex>::add_triangle(const TVertex& vertex1, const TVertex& vertex2, const TVertex& vertex3)
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
		void cmesh<TVertex>::add_line(const TVertex& vertex1, const TVertex& vertex2)
		{
			add_vertex(vertex1);
			add_last_vertex_index();

			add_vertex(vertex2);
			add_last_vertex_index();
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		void cmesh<TVertex>::add_current_vertex_index()
		{
			add_index(static_cast<index_type_t>(m_index_buffer.size()));
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		void cmesh<TVertex>::add_line(const TVertex& vertex)
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
		void cmesh<TVertex>::add_point(const TVertex& vertex)
		{
			add_vertex(vertex);
			add_last_vertex_index();
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		void cmesh<TVertex>::add_last_vertex_index()
		{
			add_index(static_cast<index_type_t>(m_index_buffer.size() - 1));
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		void cmesh<TVertex>::add_index(index_type_t index)
		{
			m_index_buffer.emplace_back(index);
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		void cmesh<TVertex>::add_vertex(const TVertex& vertex)
		{
			m_vertex_buffer.emplace_back(vertex);
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		unsigned cmesh<TVertex>::index_buffer_size() const
		{
			return SCAST(unsigned, m_index_buffer_front.size());
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVertex>
		unsigned cmesh<TVertex>::vertex_buffer_size() const
		{
			return SCAST(unsigned, m_vertex_buffer_front.size());
		}

	} //- detail

} //- sm