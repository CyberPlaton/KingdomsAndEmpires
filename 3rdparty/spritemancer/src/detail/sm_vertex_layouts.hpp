#pragma once
#include "sm_config.hpp"

namespace sm::vertexlayout
{
	//------------------------------------------------------------------------------------------------------------------------
	struct sposcolortexcoord final
	{
		static bool init();
		static bgfx::VertexLayout vertex_layout();
		static vertex_layout_handle_t vertex_layout_handle();
		static sposcolortexcoord make(float x, float y, float z, unsigned abgr, float u, float v);

		static inline bgfx::VertexLayout S_LAYOUT;
		static inline bgfx::VertexLayoutHandle S_LAYOUT_HANDLE;

		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		unsigned m_abgr = 0;
		float u = 0.0f;
		float v = 0.0f;
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct spostexcoord final
	{
		static bool init();
		static bgfx::VertexLayout vertex_layout();
		static vertex_layout_handle_t vertex_layout_handle();
		static spostexcoord make(float x, float y, float z, float u, float v);

		static inline bgfx::VertexLayout S_LAYOUT;
		static inline bgfx::VertexLayoutHandle S_LAYOUT_HANDLE;

		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float u = 0.0f;
		float v = 0.0f;
	};

} //- sm::vertexlayout