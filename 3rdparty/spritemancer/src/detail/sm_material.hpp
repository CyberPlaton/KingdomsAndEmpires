#pragma once
#include "sm_config.hpp"
#include "sm_uniform.hpp"

namespace sm
{
	namespace resource
	{
		namespace detail
		{
			//- Struct containing material data.
			//- TODO: Divide in runtime and offline data.
			//------------------------------------------------------------------------------------------------------------------------
			struct smaterial final
			{
				struct ssampler_data
				{
					string_t m_uniform_name;
					unsigned m_sampler_flags = BGFX_SAMPLER_NONE;
					bgfx::UniformHandle m_handle;
				};

				struct stexture_data
				{
					string_t m_texture_path;
					bgfx::TextureHandle m_handle;
				};

				struct sprogram_data
				{
					string_t m_program_path;
					bgfx::ProgramHandle m_handle;
				};

				struct suniform_data
				{
					string_t m_uniform_name;
					uniform_type m_type;
					bgfx::UniformHandle m_handle;
					core::cany m_data;
				};

				vector_t<suniform_data> m_uniforms;
				ssampler_data m_sampler_data;
				sprogram_data m_program_data;
				stexture_data m_texture_data;
				bgfx::ViewId m_view;
				unsigned m_state = BGFX_STATE_DEFAULT;
			};

		} //- detail

		//- Material resource class.
		//------------------------------------------------------------------------------------------------------------------------
		class cmaterial final : public core::cresource<detail::smaterial>
		{
		public:
			cmaterial() = default;
			~cmaterial();

			void bind(unsigned depth = 0);

		private:
			RTTR_ENABLE(core::cresource<detail::smaterial>);
		};

		//- Material resource manager class responsible for loading a material from file.
		//------------------------------------------------------------------------------------------------------------------------
		class cmaterial_manager final : public core::cresource_manager<cmaterial>
		{
		public:
			cmaterial_manager() = default;
			~cmaterial_manager() = default;

		protected:
			const core::resource::iresource* load(stringview_t name, const fs::cfileinfo& path) override final;

		private:
			RTTR_ENABLE(core::cresource_manager<cmaterial>);
		};
	} //- resource

} //- sm
