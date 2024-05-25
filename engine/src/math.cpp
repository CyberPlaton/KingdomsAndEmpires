#include "math.hpp"

namespace math
{
	//------------------------------------------------------------------------------------------------------------------------
	caabb::caabb()
	{
		m_aabb.lowerBound = { 0.0f, 0.0f };
		m_aabb.upperBound = { 0.0f, 0.0f };
	}

	//------------------------------------------------------------------------------------------------------------------------
	caabb::caabb(const core::srect& rect)
	{
		to_aabb(rect);
	}

	//------------------------------------------------------------------------------------------------------------------------
	caabb::caabb(const vec2_t& center, const vec2_t& halfextents)
	{
		to_aabb(center, halfextents);
	}

	//------------------------------------------------------------------------------------------------------------------------
	caabb::caabb(float x, float y, float size)
	{
		to_aabb(x, y, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	caabb::caabb(float x, float y, float halfwidth, float halfheight)
	{
		to_aabb(x, y, halfwidth, halfheight);
	}

	//------------------------------------------------------------------------------------------------------------------------
	caabb::caabb(const b2AABB& aabb) :
		m_aabb(aabb)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void caabb::to_aabb(const core::srect& rect)
	{
		to_aabb(rect.x(), rect.y(), rect.w() / 2.0f, rect.h() / 2.0f);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void caabb::to_aabb(const vec2_t& center, const vec2_t& halfextents)
	{
		to_aabb(center.x, center.y, halfextents.x, halfextents.y);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void caabb::to_aabb(float x, float y, float size)
	{
		to_aabb(x, y, size / 2.0f, size / 2.0f);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void caabb::to_aabb(float x, float y, float halfwidth, float halfheight)
	{
		CORE_NAMED_ZONE("caabb::to_aabb");

		//- Note: difference box2d internal coordinate system and engine:
		//- box2d			engine
		//- ^ Y+			------> X+
		//- |				|
		//- |				|
		//- |				|
		//- ------> X+		v Y+

		m_aabb.lowerBound = { x - halfwidth, y - halfheight };
		m_aabb.upperBound = { x + halfwidth, y + halfheight };

// 		m_aabb.lowerBound = { x, y - halfheight * 2 };
// 		m_aabb.upperBound = { x + halfwidth * 2.0f, y };
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::srect caabb::to_rect() const
	{
		CORE_NAMED_ZONE("caabb::to_rect");

		//- Note: difference box2d internal coordinate system and engine:
		//- box2d			engine
		//- ^ Y+			------> X+
		//- |				|
		//- |				|
		//- |				|
		//- ------> X+		v Y+

		const auto& c = m_aabb.GetCenter();
		const auto& he = m_aabb.GetExtents();

		return { c.x - he.x, c.y - he.y, he.x * 2.0f, he.y * 2.0f };
	}

} //- math