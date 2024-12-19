#include "sm_buffer.hpp"
#include "bgfx_integration/bgfx_integration.hpp"

namespace sm
{
	namespace buffer
	{
		namespace
		{
			//------------------------------------------------------------------------------------------------------------------------
			template<typename... ARGS>
			rttr::variant invoke_static_function(rttr::type type, rttr::string_view name, ARGS&&... args)
			{
				if (const auto m = type.get_method(name); m.is_valid())
				{
					return m.invoke({}, args...);
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
		opresult create(sbuffer* buffer)
		{
			switch (buffer->m_buffer_type)
			{
			case buffer_type_static:
			{
				if (!buffer->m_vertices.empty())
				{
					if (const auto vertex_type = rttr::type::get_by_name(buffer->m_layout_name.data()); vertex_type.is_valid())
					{
						const auto& layout = invoke_static_function(vertex_type, vertexlayout::svertex::C_VERTEX_LAYOUT_FUNC_NAME).get_value<bgfx::VertexLayout>();

						buffer->m_vbh = bgfx::createVertexBuffer(bgfx::makeRef(buffer->m_vertices.data(), buffer->m_vertices.size()),
							layout, BGFX_BUFFER_ALLOW_RESIZE).idx;

						if (!buffer->m_indices.empty())
						{
							buffer->m_ibh = bgfx::createIndexBuffer(bgfx::makeRef(buffer->m_indices.data(), buffer->m_indices.size()),
								BGFX_BUFFER_ALLOW_RESIZE).idx;
						}
					}
				}
				break;
			}
			case buffer_type_dynamic:
			{
				if (!buffer->m_vertices.empty())
				{
					if (const auto vertex_type = rttr::type::get_by_name(buffer->m_layout_name.data()); vertex_type.is_valid())
					{
						const auto& layout = invoke_static_function(vertex_type, vertexlayout::svertex::C_VERTEX_LAYOUT_FUNC_NAME).get_value<bgfx::VertexLayout>();

						buffer->m_vbh = bgfx::createDynamicVertexBuffer(bgfx::makeRef(buffer->m_vertices.data(), buffer->m_vertices.size()),
							layout, BGFX_BUFFER_ALLOW_RESIZE).idx;

						if (!buffer->m_indices.empty())
						{
							buffer->m_ibh = bgfx::createDynamicIndexBuffer(bgfx::makeRef(buffer->m_indices.data(), buffer->m_indices.size()),
								BGFX_BUFFER_ALLOW_RESIZE).idx;
						}
					}
				}
				break;
			}
			case buffer_type_transient:
			{
				if (const auto vertex_type = rttr::type::get_by_name(buffer->m_layout_name.data()); vertex_type.is_valid())
				{
					const auto& layout = invoke_static_function(vertex_type, vertexlayout::svertex::C_VERTEX_LAYOUT_FUNC_NAME).get_value<bgfx::VertexLayout>();

					if (sm::graphics::detail::check_available_transient_buffers(buffer->m_vertices.size(), layout, buffer->m_indices.size()))
					{
						//- TODO: we probably should store this somewhere for a frame, this could get destroyed when going out of scope...
						bgfx::TransientVertexBuffer tvb;
						bgfx::TransientIndexBuffer tib;

						bgfx::allocTransientBuffers(&tvb, layout, buffer->m_vertices.size(),
							&tib, buffer->m_indices.size());

						std::memcpy(tvb.data, (const void*)buffer->m_vertices.data(), sizeof(float) * buffer->m_vertices.size());
						std::memcpy(tib.data,(const void*)buffer->m_indices.data(), sizeof(index_type_t) * buffer->m_indices.size());

						buffer->m_vbh = tvb.handle.idx;
						buffer->m_ibh = tvb.handle.idx;
					}
				}
				break;
			}
			default: return opresult_fail;
			}

			return opresult_ok;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void destroy(sbuffer* buffer)
		{
			switch (buffer->m_buffer_type)
			{
			case buffer_type_static:
			{
				const auto vertex_handle = bgfx::VertexBufferHandle{ buffer->m_vbh };
				const auto index_handle = bgfx::IndexBufferHandle{ buffer->m_ibh };

				if (bgfx::isValid(vertex_handle)) bgfx::destroy(vertex_handle);
				if (bgfx::isValid(index_handle)) bgfx::destroy(index_handle);

				break;
			}
			case buffer_type_dynamic:
			{
				const auto vertex_handle = bgfx::DynamicVertexBufferHandle{ buffer->m_vbh };
				const auto index_handle = bgfx::DynamicIndexBufferHandle{ buffer->m_ibh };

				if (bgfx::isValid(vertex_handle)) bgfx::destroy(vertex_handle);
				if (bgfx::isValid(index_handle)) bgfx::destroy(index_handle);
				break;
			}
			case buffer_type_transient: break;
			default: break;
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		void update(sbuffer* buffer)
		{
			switch (buffer->m_buffer_type)
			{
			case buffer_type_static:
			{
				destroy(buffer);
				create(buffer);
				break;
			}
			case buffer_type_dynamic:
			{
				if (!buffer->m_vertices.empty())
				{
					bgfx::update(bgfx::DynamicVertexBufferHandle{ buffer->m_vbh }, 0,
						bgfx::makeRef(buffer->m_vertices.data(), buffer->m_vertices.size()));

					if (!buffer->m_indices.empty())
					{
						bgfx::update(bgfx::DynamicIndexBufferHandle{ buffer->m_ibh }, 0,
							bgfx::makeRef(buffer->m_indices.data(), buffer->m_indices.size()));
					}
				}
				break;
			}
			case buffer_type_transient:
			default: break;
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		void bind(sbuffer* buffer)
		{
			switch (buffer->m_buffer_type)
			{
			case buffer_type_transient:
			case buffer_type_static:
			{
				bgfx::setVertexBuffer(0, bgfx::VertexBufferHandle{ buffer->m_vbh });
				bgfx::setIndexBuffer(bgfx::IndexBufferHandle{ buffer->m_ibh });
				break;
			}
			case buffer_type_dynamic:
			{
				bgfx::setVertexBuffer(0, bgfx::DynamicVertexBufferHandle{ buffer->m_vbh });
				bgfx::setIndexBuffer(bgfx::DynamicIndexBufferHandle{ buffer->m_ibh });
				break;
			}
			default: break;
			}
		}

	} //- buffer

} //- sm