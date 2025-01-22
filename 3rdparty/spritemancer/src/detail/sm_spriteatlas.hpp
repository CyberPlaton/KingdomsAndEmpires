#pragma once
#include "sm_config.hpp"

namespace sm
{
	namespace resource
	{
		namespace detail
		{
			//- Struct containing spritesheet data for a texture/image.
			//------------------------------------------------------------------------------------------------------------------------
			struct sspriteatlas final
			{
				umap_t<unsigned, core::srect> m_subtextures;
				string_t m_texture_path;
				vec2_t m_size;
			};

		} //- detail

		//- Spriteatlas resource class.
		//------------------------------------------------------------------------------------------------------------------------
		class cspriteatlas final : public core::cresource<detail::sspriteatlas>
		{
		public:
			cspriteatlas() = default;
			~cspriteatlas();

			bool create(float w, float h, const vector_t<string_t>& names, const vec2_t& frames);
			const core::srect& at(stringview_t name) const;
			vec2_t dimension() const;
			unsigned subtextures() const;

			//- Routines for manual atlas creation
			cspriteatlas& begin(float w, float h);
			cspriteatlas& subtexture(stringview_t name, const core::srect& rect);
			cspriteatlas& end();

		private:
			RTTR_ENABLE(core::cresource<detail::sspriteatlas>);
		};

		//- Spriteatlas resource manager class responsible for loading a spriteatlas from file.
		//------------------------------------------------------------------------------------------------------------------------
		class cspriteatlas_manager final : public core::cresource_manager<cspriteatlas>
		{
		public:
			cspriteatlas_manager() = default;
			~cspriteatlas_manager() = default;

		protected:
			const core::resource::iresource* load(stringview_t name, const fs::cfileinfo& path) override final;

		private:
			RTTR_ENABLE(core::cresource_manager<cspriteatlas>);
		};

	} //- resource

} //- sm