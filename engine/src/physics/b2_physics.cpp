#include "b2_physics.hpp"

namespace physics
{

} //- physics

namespace algorithm
{
	//------------------------------------------------------------------------------------------------------------------------
	b2AABB aabb(const core::srect& rect)
	{
		return { {rect.x() + rect.w() / 2.0f, rect.y() + rect.h() / 2.0f},
				{rect.w() / 2.0f, rect.h() / 2.0f} };
	}

	//------------------------------------------------------------------------------------------------------------------------
	b2AABB aabb(const vec2_t& center, const vec2_t& halfextents)
	{
		b2AABB out{ { center.x - halfextents.x, center.y - halfextents.y },
					{ center.x + halfextents.x, center.y + halfextents.y } };
		return out;
	}

	//------------------------------------------------------------------------------------------------------------------------
	b2AABB aabb(float x, float y, float size)
	{
		return aabb({ x + size / 2.0f, y + size / 2.0f },
			{ size / 2.0f, size / 2.0f });
	}

	//------------------------------------------------------------------------------------------------------------------------
	b2AABB aabb(float x, float y, float w, float h)
	{
		return aabb({ x + w / 2.0f, y + h / 2.0f },
			{ w / 2.0f, h / 2.0f });
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::srect rect(const b2AABB& bbox)
	{
		const auto& c = bbox.GetCenter();
		const auto& he = bbox.GetExtents();

		return { c.x - he.x, c.y - he.y, he.x * 2.0f, he.y * 2.0f };
	}

} //- algorithm