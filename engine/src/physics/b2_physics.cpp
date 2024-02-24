#include "b2_physics.hpp"

namespace physics
{
	//------------------------------------------------------------------------------------------------------------------------
	aabb_t aabb(const core::srect& rect)
	{
		return { {rect.x() + rect.w() / 2.0f, rect.y() + rect.h() / 2.0f},
				{rect.w() / 2.0f, rect.h() / 2.0f} };
	}

	//------------------------------------------------------------------------------------------------------------------------
	aabb_t aabb(const vec2_t& center, const vec2_t& halfextents)
	{
		aabb_t out{ { center.x - halfextents.x, center.y - halfextents.y },
					{ center.x + halfextents.x, center.y + halfextents.y } };
		return out;
	}

	//------------------------------------------------------------------------------------------------------------------------
	aabb_t aabb(float x, float y, float size)
	{
		return aabb({ x + size / 2.0f, y + size / 2.0f },
			{ size / 2.0f, size / 2.0f });
	}

	//------------------------------------------------------------------------------------------------------------------------
	aabb_t aabb(float x, float y, float w, float h)
	{
		return aabb({ x + w / 2.0f, y + h / 2.0f },
			{ w / 2.0f, h / 2.0f });
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::srect rect(const aabb_t& bbox)
	{
		const auto& c = bbox.GetCenter();
		const auto& he = bbox.GetExtents();

		return { c.x - he.x, c.y - he.y, he.x * 2.0f, he.y * 2.0f };
	}

} //- physics