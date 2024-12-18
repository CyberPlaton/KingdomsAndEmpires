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
		cbuffer2::cbuffer2(rttr::type vertex_type) :
			m_vertex_type(vertex_type)
		{
			invoke_static_function(m_vertex_type, C_VERTEX_TYPE_INIT_BUFFER_FUNC_NAME, m_vertex_data);
		}

		//------------------------------------------------------------------------------------------------------------------------
		unsigned cbuffer2::vertices_count() const
		{
			return invoke_static_function(m_vertex_type, C_VERTEX_TYPE_VERTICES_COUNT_FUNC_NAME, m_vertex_data).get_value<unsigned>();
		}

		//------------------------------------------------------------------------------------------------------------------------
		unsigned cbuffer2::indices_count() const
		{

		}

		//------------------------------------------------------------------------------------------------------------------------
		bool cbuffer2::vertices_empty() const
		{
			return invoke_static_function(m_vertex_type, C_VERTEX_TYPE_VERTICES_EMPTY_FUNC_NAME, m_vertex_data).get_value<bool>();
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool cbuffer2::indices_empty() const
		{

		}

		//------------------------------------------------------------------------------------------------------------------------
		void cbuffer2::clear()
		{
			invoke_static_function(m_vertex_type, C_VERTEX_TYPE_VERTICES_CLEAR_FUNC_NAME, m_vertex_data);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cbuffer2::destroy()
		{
			invoke_static_function(m_vertex_type, C_VERTEX_TYPE_VERTICES_DESTROY_FUNC_NAME, m_vertex_data);
		}

	} //- detail

} //- sm