#pragma once
#include <bx.h>
#include <bimg.h>
#include <bgfx.h>

namespace bgfx
{
	//------------------------------------------------------------------------------------------------------------------------
	inline static bool checkAvailTransientBuffers(uint32_t _numVertices, const bgfx::VertexLayout& _layout, uint32_t _numIndices)
	{
		return _numVertices == bgfx::getAvailTransientVertexBuffer(_numVertices, _layout)
			&& (0 == _numIndices || _numIndices == bgfx::getAvailTransientIndexBuffer(_numIndices))
			;
	}

} //- bgfx
