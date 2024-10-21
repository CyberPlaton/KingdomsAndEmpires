#pragma once
#include "core_config.hpp"

namespace core
{
	//------------------------------------------------------------------------------------------------------------------------
	struct srect final
	{
		srect(const vec2_t& xy, const vec2_t& wh);
		srect(float x = 0.0f, float y = 0.0f, float w = 0.0f, float h = 0.0f);

		float x() const { return m_x; };
		float y() const { return m_y; };
		float w() const { return m_w; };
		float h() const { return m_h; };

		vec2_t top_left() const { return { m_x, m_y }; }
		vec2_t top_right() const { return { m_x + m_w, m_y }; }
		vec2_t bottom_left() const { return { m_x, m_y + m_h }; }
		vec2_t bottom_right() const { return { m_x + m_w, m_y + m_h }; }

		void set(float x, float y, float w, float h);
		void position(float x, float y);
		void dimension(float w, float h);

		float m_x, m_y, m_w, m_h;

		RTTR_ENABLE();
	};

} //- core