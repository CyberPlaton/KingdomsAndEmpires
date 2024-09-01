#pragma once
#include <core.h>
#include "bgfx_integration/bgfx.hpp"
#include "sm_config.hpp"

namespace sm
{
	namespace vertex_layouts
	{
		//- Vertex with positions and texture coordinates
		//------------------------------------------------------------------------------------------------------------------------
		struct spostexcoord final
		{
			static bool init();
			static spostexcoord make(float x, float y, float u, float v);

			inline static bgfx::VertexLayout S_LAYOUT;
			inline static bgfx::VertexLayoutHandle S_HANDLE;

			vec3_t m_position;
			vec2_t m_uv;
		};

		//- Vertex with positions, texture coordinates and colors
		//------------------------------------------------------------------------------------------------------------------------
		struct spostexcoordcolor final
		{
			static bool init();
			static spostexcoordcolor make(float x, float y, float u, float v, unsigned abgr);

			inline static bgfx::VertexLayout S_LAYOUT;
			inline static bgfx::VertexLayoutHandle S_HANDLE;

			vec3_t m_position;
			vec2_t m_uv;
			unsigned m_abgr;
		};

	} //- vertex_layouts

	//- Responsible for storing vertices and operating on them.
	//- Note: when having a static vertex buffer, then first bind call will create a buffer with the vertices already provided,
	//- other types will update the dynamic vertex buffer or allocate a transient vertex buffer for pushed vertices.
	//------------------------------------------------------------------------------------------------------------------------
	template<typename TVertex>
	class cvertices final
	{
	public:
		explicit cvertices(const vertex_type type, const bgfx::VertexLayout& layout, const bgfx::VertexLayoutHandle handle,
			const unsigned reserve = 0, const unsigned flags = BGFX_BUFFER_NONE);
		~cvertices();

		void bind(uint8_t stream = 0, unsigned vertex_start = 0, unsigned vertex_count = MAX(unsigned));

		cvertices& push(TVertex&& vertex);
		template<typename... ARGS>
		cvertices& push(ARGS&&... args);

		unsigned size() const { return static_cast<unsigned>(m_vertices.size()); }

	private:
		bgfx::VertexLayout& m_layout;
		vector_t<TVertex> m_vertices;
		bgfx::VertexBufferHandle m_handle;
		bgfx::VertexLayoutHandle m_layout_handle;
		unsigned m_flags;
		vertex_type m_type;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TVertex>
	sm::cvertices<TVertex>::~cvertices()
	{
		if (bgfx::isValid(m_handle))
		{
			bgfx::destroy(m_handle);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TVertex>
	void sm::cvertices<TVertex>::bind(uint8_t stream /*= 0*/, unsigned vertex_start /*= 0*/, unsigned vertex_count /*= MAX(unsigned)*/)
	{
		if (size() > 0)
		{
			const auto count = size();

			switch (m_type)
			{
			default:
			case vertex_type_none:
			{
				return;
			}
			//- For static and dynamic vertex buffers only set is required, creation is done somewhere else
			case vertex_type_static:
			{
				if (!bgfx::isValid(m_handle))
				{
					CORE_ASSERT(size() > 0, "Invalid operation. Vertex buffer is empty on first submission!");

					m_handle = bgfx::createVertexBuffer(bgfx::makeRef(m_vertices, size()), m_layout, m_flags);
				}
				break;
			}
			case vertex_type_dynamic:
			{
				if (!bgfx::isValid(m_handle))
				{
					m_handle = bgfx::createDynamicVertexBuffer(bgfx::makeRef(m_vertices, size()), m_layout, m_flags);
				}
				else
				{
					bgfx::update(m_handle, 0, bgfx::makeRef(m_vertices, size()));
				}
				break;
			}
			case vertex_type_transient:
			{
				if (count < bgfx::getAvailTransientVertexBuffer(count, m_layout))
				{
					bgfx::TransientVertexBuffer buffer;

					bgfx::allocTransientVertexBuffer(&buffer, count, m_layout);

					auto* verts = (TVertex*)buffer.data;

					bx::memCopy(verts, m_vertices.data(), count * sizeof(TVertex));

					m_handle = buffer.handle;
				}
				break;
			}
			}

			bgfx::setVertexBuffer(stream, m_handle, vertex_start, vertex_count, m_layout_handle);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TVertex>
	cvertices<TVertex>& sm::cvertices<TVertex>::push(TVertex&& vertex)
	{
		m_vertices.emplace_back(std::move(vertex));
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TVertex>
	template<typename... ARGS>
	cvertices<TVertex>& sm::cvertices<TVertex>::push(ARGS&&... args)
	{
		m_vertices.emplace_back(TVertex::make(std::forward<ARGS>(args)...));
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TVertex>
	sm::cvertices<TVertex>::cvertices(const vertex_type type, const bgfx::VertexLayout& layout, const bgfx::VertexLayoutHandle handle,
		const unsigned reserve /*= 0*/, const unsigned flags /*= BGFX_BUFFER_NONE*/) : m_layout(layout), m_type(type), m_layout_handle(handle), m_flags(flags)
	{
		m_vertices.reserve(reserve);
	}

} //- sm