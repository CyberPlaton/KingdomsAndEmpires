#include "core_color.hpp"

namespace core
{
	//------------------------------------------------------------------------------------------------------------------------
	scolor::scolor(common_color color) :
		m_r(0), m_g(0), m_b(0), m_a(0)
	{
		constexpr array_t<uint8_t, 4> C_COMMON_COLORS[] =
		{
			{134, 38, 51, 255},
			{239, 83, 80, 255},
			{255, 205, 210, 255},

			{245, 124, 0, 255},
			{255, 171, 64, 255},
			{255, 224, 178, 255},

			{251, 192, 45, 255},
			{255, 235, 59, 255},
			{255, 245, 157, 255},

			{52, 168, 83, 255},
			{129, 199, 132, 255},
			{200, 230, 201, 255},

			{0, 188, 212, 255},
			{77, 208, 225, 255},
			{178, 235, 242, 255},

			{33, 150, 243, 255},
			{144, 202, 249, 255},
			{187, 222, 251, 255},

			{156, 39, 176, 255},
			{233, 30, 99, 255},
			{248, 187, 208, 255},

			{233, 30, 99, 255},
			{240, 98, 146, 255},
			{248, 187, 208, 255},

			{255, 255, 255, 255},
			{0, 0, 0, 255},
			{33, 33, 33, 255},
			{66, 66, 66, 255},
			{117, 117, 117, 255},
			{153, 153, 153, 255},
			{189, 189, 189, 255},
			{224, 224, 224, 255},
			{238, 238, 238, 255},
			{255, 255, 255, 255},
		};

		const auto& c = C_COMMON_COLORS[(unsigned)color];
		m_r = c[0];
		m_g = c[1];
		m_b = c[2];
		m_a = c[3];
	}

	//------------------------------------------------------------------------------------------------------------------------
	scolor::scolor(uint8_t r /*= 0*/, uint8_t g /*= 0*/, uint8_t b /*= 0*/, uint8_t a /*= 0*/) :
		m_r(r), m_g(g), m_b(b), m_a(a)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	scolor::scolor(vec4_t normalized) :
		m_r(normalized.r * 255),
		m_g(normalized.g * 255),
		m_b(normalized.b * 255),
		m_a(normalized.a * 255)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	vec4_t scolor::normalize() const
	{
		return { m_r / 255.0f, m_g / 255.0f, m_b / 255.0f, m_a / 255.0f };
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned scolor::rgba() const
	{
		return ((SCAST(unsigned, m_r) << 24) | (SCAST(unsigned, m_g) << 16) | (SCAST(unsigned, m_b) << 8) | SCAST(unsigned, m_a));
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned scolor::abgr() const
	{
		return ((SCAST(unsigned, m_a) << 24) | (SCAST(unsigned, m_b) << 16) | (SCAST(unsigned, m_g) << 8) | SCAST(unsigned, m_r));
	}

} //- core