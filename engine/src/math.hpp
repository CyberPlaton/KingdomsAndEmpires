#pragma once
#include <core.h>
#include "physics/b2_physics.hpp"

namespace math
{
	constexpr mat4_t C_MAT4_ID				= mat4_t(1.0f);
	constexpr vec2_t C_VEC_UP				= vec2_t(0.0f, -1.0f);
	constexpr vec2_t C_VEC_RIGHT			= vec2_t(1.0f, 0.0f);
	constexpr float C_ALMOST_EQUAL_EPSILON	= 0.04f;

	bool almost_equal(float a, float b, float e = C_ALMOST_EQUAL_EPSILON);
	bool almost_equal(const vec2_t& a, const vec2_t& b, float e = C_ALMOST_EQUAL_EPSILON);

	//- Transforms given position, scale, shear and rotation and returns world space transform matrix
	//- TODO: shear
	//------------------------------------------------------------------------------------------------------------------------
	void transform(mat4_t& out, const vec2_t& position, const vec2_t& scale, const vec2_t& shear,
		float rotation, const mat4_t& parent = C_MAT4_ID);

	//- Transforms given position, scale, shear and rotation and returns final world space position, scale and rotation
	//- ready to be used for rendering
	//------------------------------------------------------------------------------------------------------------------------
	std::tuple<vec2_t, vec2_t, float> transform(const vec2_t& position, const vec2_t& scale, const vec2_t& shear,
		float rotation, const mat4_t& parent = C_MAT4_ID);

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