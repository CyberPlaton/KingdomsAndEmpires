#pragma once
#include "core_config.hpp"

namespace math
{
	constexpr mat4_t C_MAT4_ID = mat4_t(1.0f);
	constexpr vec2_t C_VEC_UP = vec2_t(0.0f, -1.0f);
	constexpr vec2_t C_VEC_RIGHT = vec2_t(1.0f, 0.0f);
	constexpr float C_ALMOST_EQUAL_EPSILON = 0.04f;
	constexpr float C_EPSILON = 0.0000001f;
	constexpr float C_PI = 3.1415927f;
	constexpr float C_PI2 = C_PI * C_PI;

	bool inbetween(float value, float min, float max);
	bool almost_equal(float a, float b, float e = C_ALMOST_EQUAL_EPSILON);
	bool almost_equal(const vec2_t& a, const vec2_t& b, float e = C_ALMOST_EQUAL_EPSILON);
	vec2_t rotate_point_around_origin(const vec2_t& point, const vec2_t& origin, const float degrees);
	mat4_t transform(const vec2_t& translation, const vec2_t& scale, const vec2_t& shear, float rotation, const mat4_t& parent = mat4_t(1.0f));
	vec2_t extract_translation(const mat4_t& matrix);
	vec2_t extract_scale(const mat4_t& matrix);
	vec2_t extract_shear(const mat4_t& matrix);
	float extract_rotation(const mat4_t& matrix);

	//- Convert from world coordinates to screen coordinates
	vec2_t project_2d(const vec2_t& point, const mat4_t& camera_matrix);

	//- Convert from screen coordinates to world coordinates.
	//- Note: we invert the matrix inside this function.
	vec2_t unproject_2d(const vec2_t& point, const mat4_t& camera_matrix);

} //- math