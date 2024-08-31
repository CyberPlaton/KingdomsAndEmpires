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

	//------------------------------------------------------------------------------------------------------------------------
	inline static uint32_t encodeNormalRgba8(float _x, float _y = 0.0f, float _z = 0.0f, float _w = 0.0f)
	{
		const float src[] =
		{
			_x * 0.5f + 0.5f,
			_y * 0.5f + 0.5f,
			_z * 0.5f + 0.5f,
			_w * 0.5f + 0.5f,
		};
		uint32_t dst;
		bx::packRgba8(&dst, src);
		return dst;
	}

} //- bgfx