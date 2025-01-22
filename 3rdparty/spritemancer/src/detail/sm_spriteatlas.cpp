#include "sm_spriteatlas.hpp"

namespace sm
{
	namespace resource
	{
		//------------------------------------------------------------------------------------------------------------------------
		bool cspriteatlas::create(float w, float h, const vector_t<string_t>& names, const vec2_t& frames)
		{
			const auto _w = (float)w;
			const auto _h = (float)h;
			const auto framesize = vec2_t(_w / frames.x, _h / frames.y);

			begin(w, h);

			for (auto y = 0u; y < frames.y; ++y)
			{
				for (auto x = 0u; x < frames.x; ++x)
				{
					const auto px = x * framesize.x / _w;
					const auto py = y * framesize.y / _h;
					const auto pw = framesize.x / _w;
					const auto ph = framesize.y / _h;

					subtexture(names[x + y], { px, py, pw, ph });
				}
			}

			end();

			return true;
		}

		//------------------------------------------------------------------------------------------------------------------------
		const core::srect& cspriteatlas::at(stringview_t name) const
		{
			return m_resource.m_subtextures.at(algorithm::hash(name));
		}

		//------------------------------------------------------------------------------------------------------------------------
		vec2_t cspriteatlas::dimension() const
		{
			return m_resource.m_size;
		}

		//------------------------------------------------------------------------------------------------------------------------
		unsigned cspriteatlas::subtextures() const
		{
			return static_cast<unsigned>(m_resource.m_subtextures.size());
		}

		//------------------------------------------------------------------------------------------------------------------------
		cspriteatlas& cspriteatlas::begin(float w, float h)
		{
			m_resource.m_size = { (float)w, (float)h };
			return *this;
		}

		//------------------------------------------------------------------------------------------------------------------------
		cspriteatlas& cspriteatlas::subtexture(stringview_t name, const core::srect& rect)
		{
			m_resource.m_subtextures[algorithm::hash(name)] = rect;
			return *this;
		}

		//------------------------------------------------------------------------------------------------------------------------
		cspriteatlas& cspriteatlas::end()
		{
			CORE_ASSERT(!m_resource.m_subtextures.empty() && m_resource.m_size.x > 0.0f && m_resource.m_size.y > 0.0f,
				"Invalid operation. An empty spriteatlas is not valid");
			return *this;
		}

	} //- resource

} //- sm

RTTR_REGISTRATION
{
	using namespace sm::resource;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::cregistrator<detail::sspriteatlas>("sspriteatlas")
		.prop("m_subtextures", &detail::sspriteatlas::m_subtextures)
		.prop("m_texture_path", &detail::sspriteatlas::m_texture_path)
		.prop("m_size", &detail::sspriteatlas::m_size)
		;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::registration::class_<cspriteatlas_manager>("cspriteatlas_manager")
	.constructor<>()
	(
		rttr::policy::ctor::as_raw_ptr
	)
	;

}
