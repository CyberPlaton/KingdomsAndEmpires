#pragma once
#include <core.h>
#include "physics/b2_physics.hpp"

namespace math
{
	//- AABB class. Assuming the origin of the rectangle is centered.
	//------------------------------------------------------------------------------------------------------------------------
	class caabb
	{
	public:
		caabb();
		~caabb() = default;
		caabb(const b2AABB& aabb);
		caabb(const core::srect& rect);
		caabb(const vec2_t& center, const vec2_t& halfextents);
		caabb(float x, float y, float size);
		caabb(float x, float y, float halfwidth, float halfheight);

		void to_aabb(const core::srect& rect);
		void to_aabb(const vec2_t& center, const vec2_t& halfextents);
		void to_aabb(float x, float y, float size);
		void to_aabb(float x, float y, float halfwidth, float halfheight);

		core::srect to_rect() const;

		operator b2AABB () const { return m_aabb; }
		operator core::srect() const { return to_rect(); }

		b2AABB m_aabb;
	};

} //- math