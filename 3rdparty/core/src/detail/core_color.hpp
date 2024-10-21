#pragma once
#include "core_config.hpp"

namespace core
{
	//------------------------------------------------------------------------------------------------------------------------
	struct scolor final
	{
		scolor(common_color color);
		scolor(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 0);
		scolor(vec4_t normalized);

		inline uint8_t r() const { return m_r; }
		inline uint8_t g() const { return m_g; }
		inline uint8_t b() const { return m_b; }
		inline uint8_t a() const { return m_a; }

		template<typename T>
		inline T cliteral() const { return { m_r, m_g, m_b, m_a }; }

		vec4_t normalize() const;

		unsigned rgba() const;	//- Useful conversion for raylib etc.
		unsigned abgr() const;	//- Useful conversion for ImGui

		uint8_t m_r, m_g, m_b, m_a;

		RTTR_ENABLE();
	};

} //- core