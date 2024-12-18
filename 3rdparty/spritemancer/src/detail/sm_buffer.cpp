#include "sm_buffer.hpp"

namespace sm
{
	namespace detail
	{
		namespace
		{
			//------------------------------------------------------------------------------------------------------------------------
			template<typename... ARGS>
			rttr::variant invoke_static_function(rttr::type type, rttr::string_view name, core::cany& buffer, ARGS&&... args)
			{
				if (const auto m = type.get_method(name); m.is_valid())
				{
					return m.invoke({}, buffer, args...);
				}
				else
				{
					log_error(fmt::format("Failed invoking unknown function '{}' in type '{}'",
						name.data(), type.get_name().data()));
				}
				return {};
			}

		} //- unnamed

		//------------------------------------------------------------------------------------------------------------------------
		cbuffer::cbuffer(rttr::type vertex_type) :
			m_vertex_type(vertex_type)
		{
			invoke_static_function(m_vertex_type, C_VERTEX_TYPE_INIT_BUFFER_FUNC_NAME, m_vertex_data);
		}

		//------------------------------------------------------------------------------------------------------------------------
		cbuffer& cbuffer::push_vertex(const core::cany& vertex)
		{
			invoke_static_function(m_vertex_type, C_VERTEX_TYPE_VERTEX_EMPTY_FUNC_NAME, m_vertex_data, vertex);
			return *this;
		}

		//------------------------------------------------------------------------------------------------------------------------
		cbuffer& cbuffer::push_index(index_type_t index)
		{
			m_indices.emplace_back(index);
			return *this;
		}

		//------------------------------------------------------------------------------------------------------------------------
		cbuffer& cbuffer::push_line(const core::cany& vertex)
		{
			if (vertices_empty())
			{
				push_last_index();
				push_current_index();
			}
			push_vertex(vertex);
			return *this;
		}

		//------------------------------------------------------------------------------------------------------------------------
		cbuffer& cbuffer::push_line(const core::cany& vertex1, const core::cany& vertex2)
		{
			push_vertex(vertex1);
			push_last_index();

			push_vertex(vertex2);
			push_last_index();
			return *this;
		}

		//------------------------------------------------------------------------------------------------------------------------
		cbuffer& cbuffer::push_point(const core::cany& vertex)
		{
			push_vertex(vertex);
			push_last_index();
			return *this;
		}

		//------------------------------------------------------------------------------------------------------------------------
		cbuffer& cbuffer::push_triangle(const core::cany& vertex1, const core::cany& vertex2, const core::cany& vertex3)
		{
			push_vertex(vertex1);
			push_vertex(vertex2);
			push_vertex(vertex3);

			const auto i = SCAST(index_type_t, m_indices.size());
			push_index(i);
			push_index(i + 1);
			push_index(i + 2);
			return *this;
		}

		//------------------------------------------------------------------------------------------------------------------------
		cbuffer& cbuffer::push_quad(const core::cany& vertex1, const core::cany& vertex2, const core::cany& vertex3, const core::cany& vertex4)
		{
			push_vertex(vertex1);
			push_vertex(vertex2);
			push_vertex(vertex3);
			push_vertex(vertex4);

			const auto i = SCAST(index_type_t, m_indices.size());
			push_index(i);
			push_index(i + 1);
			push_index(i + 2);
			push_index(i + 3);
			push_index(i + 1);
			push_index(i + 3);
			push_index(i + 2);
			return *this;
		}

		//------------------------------------------------------------------------------------------------------------------------
		unsigned cbuffer::vertices_count() const
		{
			const auto result = invoke_static_function(m_vertex_type, C_VERTEX_TYPE_VERTEX_COUNT_FUNC_NAME, const_cast<core::cany&>(m_vertex_data));

			return result.get_value<unsigned>();
		}

		//------------------------------------------------------------------------------------------------------------------------
		unsigned cbuffer::indices_count() const
		{
			return SCAST(unsigned, m_indices.size());
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool cbuffer::vertices_empty() const
		{
			const auto result = invoke_static_function(m_vertex_type, C_VERTEX_TYPE_VERTEX_EMPTY_FUNC_NAME, const_cast<core::cany&>(m_vertex_data));

			return result.get_value<bool>();
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool cbuffer::indices_empty() const
		{
			return m_indices.empty();
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool cbuffer::valid() const
		{
			return bgfx::isValid(bgfx::VertexBufferHandle{ vertex_buffer() }) &&
				bgfx::isValid(bgfx::IndexBufferHandle{ index_buffer() });
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cbuffer::submit()
		{
			switch (m_buffer_type)
			{
			case detail::buffer_type_static:
			{
				destroy();
				create_buffers();
				break;
			}
			case detail::buffer_type_dynamic:
			{
				if (!valid())
				{
					create_buffers();
				}
				else
				{
					invoke_static_function(m_vertex_type, C_VERTEX_TYPE_VERTEX_BUFFER_UPDATE_FUNC_NAME, m_vertex_data, m_vertex_buffer_handle, m_buffer_type, 0);

					bgfx::update(bgfx::DynamicIndexBufferHandle{ index_buffer() }, 0, bgfx::makeRef(m_indices.data(), indices_count()));
				}
				break;
			}
			default:
			case detail::buffer_type_transient:
			{
				break;
			}
			}

			swap_buffers();
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cbuffer::clear()
		{
			invoke_static_function(m_vertex_type, C_VERTEX_TYPE_VERTEX_CLEAR_FUNC_NAME, m_vertex_data);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cbuffer::destroy()
		{
			invoke_static_function(m_vertex_type, C_VERTEX_TYPE_VERTEX_DESTROY_FUNC_NAME, m_vertex_data);

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
		void cbuffer::create_buffers()
		{
			invoke_static_function(m_vertex_type, C_VERTEX_TYPE_VERTEX_BUFFER_CREATE_FUNC_NAME, m_vertex_data, m_vertex_buffer_handle, m_buffer_type);

			switch (m_buffer_type)
			{
			case detail::buffer_type_static:
			{
				if (!vertices_empty() && !indices_empty())
				{
					m_index_buffer_handle = bgfx::createIndexBuffer(bgfx::makeRef(m_indices.data(), indices_count())).idx;
				}
				break;
			}
			case detail::buffer_type_dynamic:
			{
				if (!vertices_empty() && !indices_empty())
				{
					m_index_buffer_handle = bgfx::createDynamicIndexBuffer(bgfx::makeRef(m_indices.data(), indices_count()),
						BGFX_BUFFER_ALLOW_RESIZE).idx;
				}
				break;
			}
			default:
			case detail::buffer_type_transient:
			{
				break;
			}
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cbuffer::push_last_index()
		{
			m_indices.emplace_back(static_cast<index_type_t>(m_indices.size() - 1));
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cbuffer::push_current_index()
		{
			m_indices.emplace_back(static_cast<index_type_t>(m_indices.size()));
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cbuffer::swap_buffers()
		{
			invoke_static_function(m_vertex_type, C_VERTEX_TYPE_VERTEX_SWAP_BUFFERS_FUNC_NAME, m_vertex_data, m_vertex_data_front);
		}

	} //- detail

} //- sm