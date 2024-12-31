#include "core_math.hpp"

namespace math
{
	//------------------------------------------------------------------------------------------------------------------------
	bool inbetween(float value, float min, float max)
	{
		return value >= min && value <= max;
	}

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
	vec2_t rotate_point_around_origin(const vec2_t& point, const vec2_t& origin, const float degrees)
	{
		const float rad = glm::radians(degrees);

		//- Calculate the sine and cosine of the rotation angle
		const float cos_theta = glm::cos(rad);
		const float sin_theta = glm::sin(rad);

		//- Translate point back to origin
		const float translated_x = point.x - origin.x;
		const float translated_y = point.y - origin.y;

		//- Rotate point
		const float rotated_x = translated_x * cos_theta - translated_y * sin_theta;
		const float rotated_y = translated_x * sin_theta + translated_y * cos_theta;

		//- Translate point back to its original position
		return { rotated_x + origin.x, rotated_y + origin.y };
	}

	//------------------------------------------------------------------------------------------------------------------------
	mat4_t transform(const vec2_t& translation, const vec2_t& scale, const vec2_t& shear, float rotation,
		const mat4_t& parent /*= mat4_t(1.0f)*/)
	{
		static constexpr vec3_t C_Z = { 0.0f, 0.0f, 1.0f };

		mat4_t shear_matrix = C_MAT4_ID;
		shear_matrix[0][1] = shear.x; //- Shear in x direction
		shear_matrix[1][0] = shear.y; //- Shear in y direction

		return parent
			* glm::translate(C_MAT4_ID, vec3_t(translation, 0.0f))
			* glm::rotate(C_MAT4_ID, rotation, C_Z)
			* shear_matrix
			* glm::scale(C_MAT4_ID, vec3_t(scale, 0.0f));
	}

	//------------------------------------------------------------------------------------------------------------------------
	vec2_t extract_translation(const mat4_t& matrix)
	{
		const auto& p = vec3_t(matrix[3]);

		return { p.x, p.y };
	}

	//------------------------------------------------------------------------------------------------------------------------
	vec2_t extract_scale(const mat4_t& matrix)
	{
		return
		{
			glm::length(vec3_t(matrix[0])),
			glm::length(vec3_t(matrix[1]))
		};
	}

	//------------------------------------------------------------------------------------------------------------------------
	vec2_t extract_shear(const mat4_t& matrix)
	{
		float x = glm::length(vec3_t(matrix[0]));
		float y = glm::length(vec3_t(matrix[1]));

		return
		{
			glm::dot(vec3_t(matrix[0]), vec3_t(matrix[1])) / (x * y),
			glm::dot(vec3_t(matrix[1]), vec3_t(matrix[0])) / (x * y)
		};
	}

	//------------------------------------------------------------------------------------------------------------------------
	float extract_rotation(const mat4_t& matrix)
	{
		float x, y, z;

		glm::extractEulerAngleXYZ(matrix, x, y, z);

		return z;
	}

	//------------------------------------------------------------------------------------------------------------------------
	vec2_t project_2d(const vec2_t& point, const mat4_t& camera_matrix)
	{
		float px = point.x;
		float py = point.y;
		float pw = 1.0f;

		float x = camera_matrix[0][0] * px + camera_matrix[1][0] * py + camera_matrix[3][0] * pw;
		float y = camera_matrix[0][1] * px + camera_matrix[1][1] * py + camera_matrix[3][1] * pw;
		float w = camera_matrix[0][3] * px + camera_matrix[1][3] * py + camera_matrix[3][3] * pw;

		return { x, y };
	}

	//------------------------------------------------------------------------------------------------------------------------
	vec2_t unproject_2d(const vec2_t& point, const mat4_t& camera_matrix)
	{
		const auto inverse = glm::inverse(camera_matrix);
		float px = point.x;
		float py = point.y;
		float pw = 1.0f;

		float x = inverse[0][0] * px + inverse[1][0] * py + inverse[3][0] * pw;
		float y = inverse[0][1] * px + inverse[1][1] * py + inverse[3][1] * pw;
		float w = inverse[0][3] * px + inverse[1][3] * py + inverse[3][3] * pw;

		if (w != 0.0f)
		{
			x /= w;
			y /= w;
		}

		return { x, y };
	}

} //- math