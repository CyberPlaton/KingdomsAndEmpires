#pragma once
#include "detail/sm_config.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	class cspriteatlas final : public core::cresource
	{
	public:
		static void destroy(cspriteatlas& atlas);

		explicit cspriteatlas(unsigned w, unsigned h, const vector_t<string_t>& names, const vec2_t& frames);
		explicit cspriteatlas(const cspriteatlas& other);
		cspriteatlas();
		~cspriteatlas() = default;

		opresult create(unsigned w, unsigned h, const vector_t<string_t>& names, const vec2_t& frames);
		const core::srect& at(stringview_t name) const;
		vec2_t dimension() const;
		unsigned subtextures() const;

		//- Routines for manual atlas creation
		cspriteatlas& begin(unsigned w, unsigned h);
		cspriteatlas& subtexture(stringview_t name, const core::srect& rect);
		cspriteatlas& end();

	private:
		umap_t<unsigned, core::srect> m_subtextures;
		vec2_t m_size;

		RTTR_ENABLE(core::cresource);
	};

} //- sm