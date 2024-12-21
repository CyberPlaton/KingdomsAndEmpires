#pragma once
#include "sm_config.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	class cspriteatlas final : public core::cresource
	{
	public:
		static void destroy(cspriteatlas& atlas);

		explicit cspriteatlas(unsigned w, unsigned h, const vector_t<string_t>& names, const vec2_t& frames);
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

	//------------------------------------------------------------------------------------------------------------------------
	class cspriteatlas_manager final :
		public core::cservice,
		public core::cresource_manager<cspriteatlas>
	{
	public:
		cspriteatlas_manager(unsigned reserve = C_SPRITEATLAS_RESOURCE_MANAGER_RESERVE_COUNT);
		~cspriteatlas_manager();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float) override final;

		spriteatlas_handle_t load_sync(stringview_t name, unsigned w, unsigned h, const vector_t<string_t>& names, const vec2_t& frames);
		spriteatlas_handle_t load_sync(stringview_t name, const cspriteatlas& other);

		core::cfuture_type<spriteatlas_handle_t> load_async(stringview_t name, unsigned w, unsigned h, const vector_t<string_t>& names, const vec2_t& frames);
		core::cfuture_type<spriteatlas_handle_t> load_async(stringview_t name, const cspriteatlas& other);

	private:
		RTTR_ENABLE(core::cservice, core::cresource_manager<cspriteatlas>);
	};

} //- sm