#include "math.hpp"

namespace math
{
	//------------------------------------------------------------------------------------------------------------------------
	bool almost_equal(float a, float b, float e /*= C_ALMOST_EQUAL_EPSILON*/)
	{
		return glm::distance(glm::abs(a), glm::abs(b)) < C_ALMOST_EQUAL_EPSILON;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool almost_equal(const vec2_t& a, const vec2_t& b, float e /*= C_ALMOST_EQUAL_EPSILON*/)
	{
		return almost_equal(a.x, b.x, e) && almost_equal(a.y, b.y, e);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void transform(mat4_t& out, const vec2_t& position, const vec2_t& scale, const vec2_t& shear,
		float rotation, const mat4_t& parent /*= C_MAT4_ID*/)
	{
		static vec3_t S_POSITION = vec3_t(0.0f, 0.0f, 1.0f), S_DIMENSION = vec3_t(0.0f, 0.0f, 1.0f),
			S_Z_AXIS = vec3_t(0.0f, 0.0f, 1.0f);
		static float S_ROTATION = 0.0f;

		S_POSITION.x = position.x;
		S_POSITION.y = position.y;
		S_DIMENSION.x = scale.x;
		S_DIMENSION.y = scale.y;
		S_ROTATION = glm::radians(rotation);

		out = glm::translate(C_MAT4_ID, S_POSITION) *
			glm::rotate(C_MAT4_ID, S_ROTATION, S_Z_AXIS) *
			glm::scale(C_MAT4_ID, S_DIMENSION);
	}

	//------------------------------------------------------------------------------------------------------------------------
	std::tuple<vec2_t, vec2_t, float> transform(const vec2_t& position, const vec2_t& scale, const vec2_t& shear,
		float rotation, const mat4_t& parent /*= C_MAT4_ID*/)
	{
		static vec3_t S_EULER_ANGLES = vec3_t(0.0f);
		static mat4_t S_TRANSFORM = mat4_t(1.0f);

		transform(S_TRANSFORM, position, scale, shear, rotation, parent);

		vec2_t p, s;
		float r;

		//- extract 2D position
		p.x = S_TRANSFORM[3].x;
		p.y = S_TRANSFORM[3].y;

		//- extract 2D scale
		s.x = glm::length(S_TRANSFORM[0]);
		s.y = glm::length(S_TRANSFORM[1]);

		//- extract rotation around Z axis
		glm::extractEulerAngleXYZ(S_TRANSFORM, S_EULER_ANGLES.x, S_EULER_ANGLES.y, S_EULER_ANGLES.z);
		r = S_EULER_ANGLES.z;

		return {p, s, r};
	}

} //- math