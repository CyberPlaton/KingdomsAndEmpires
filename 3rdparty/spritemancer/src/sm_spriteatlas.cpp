#include "sm_spriteatlas.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	void cspriteatlas::destroy(cspriteatlas& /*atlas*/)
	{
		//- noop
	}

	//------------------------------------------------------------------------------------------------------------------------
	cspriteatlas::cspriteatlas(unsigned w, unsigned h, const vector_t<string_t>& names, const vec2_t& frames)
	{
		create(w, h, names, frames);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cspriteatlas::cspriteatlas() :
		m_size({ 0.0f, 0.0f })
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cspriteatlas::cspriteatlas(const cspriteatlas& other) :
		m_size(other.m_size), m_subtextures(other.m_subtextures)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cspriteatlas::create(unsigned w, unsigned h, const vector_t<string_t>& names, const vec2_t& frames)
	{
		const auto _w = (float)w;
		const auto _h = (float)h;
		const auto framesize = vec2_t(_w / frames.x, _h / frames.y);

		begin(w, h);

		auto name_idx = 0;
		for (auto y = 0u; y < frames.y; ++y)
		{
			for (auto x = 0u; x < frames.x; ++x)
			{
				const auto px = x * framesize.x / _w;
				const auto py = y * framesize.y / _h;
				const auto pw = framesize.x / _w;
				const auto ph = framesize.y / _h;

				subtexture(names[name_idx++], { px, py, pw, ph });
			}
		}

		end();

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	const core::srect& cspriteatlas::at(stringview_t name) const
	{
		if (const auto it = m_subtextures.find(algorithm::hash(name)); it != m_subtextures.end())
		{
			return it->second;
		}

		return { 0.0f, 0.0f, 1.0f, 1.0f };
	}

	//------------------------------------------------------------------------------------------------------------------------
	vec2_t cspriteatlas::dimension() const
	{
		return m_size;
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cspriteatlas::subtextures() const
	{
		return static_cast<unsigned>(m_subtextures.size());
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::cspriteatlas& cspriteatlas::begin(unsigned w, unsigned h)
	{
		m_size = { (float)w, (float)h };
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::cspriteatlas& cspriteatlas::subtexture(stringview_t name, const core::srect& rect)
	{
		m_subtextures[algorithm::hash(name)] = rect;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::cspriteatlas& cspriteatlas::end()
	{
		CORE_ASSERT(!m_subtextures.empty() && m_size.x > 0.0f && m_size.y > 0.0f, "Invalid operation. An empty spriteatlas is not valid");
		return *this;
	}

} //- sm