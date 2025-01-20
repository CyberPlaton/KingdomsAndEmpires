#pragma once
#include "sm_config.hpp"

namespace sm
{
	namespace resource
	{
		namespace detail
		{
			//- Struct containing a program combining a vertex and pixel shader or compute shader.
			//------------------------------------------------------------------------------------------------------------------------
			struct sprogram final
			{
				string_t m_vertex_shader_path;
				string_t m_pixel_shader_path;
				string_t m_compute_shader_path;
				bgfx::ProgramHandle m_handle;
			};

		} //- detail

		//- Program resource class.
		//------------------------------------------------------------------------------------------------------------------------
		class cprogram final : public core::cresource<detail::sprogram>
		{
		public:
			cprogram() = default;
			~cprogram();

		private:
			RTTR_ENABLE(core::cresource<detail::sprogram>);
		};

		//- Program resource manager class responsible for loading a program from shaders or one shader.
		//------------------------------------------------------------------------------------------------------------------------
		class cprogram_manager final : public core::cresource_manager<cprogram>
		{
		public:
			cprogram_manager() = default;
			~cprogram_manager() = default;

		protected:
			const core::resource::iresource* load(stringview_t name, const fs::cfileinfo& path) override final;

		private:
			RTTR_ENABLE(core::cresource_manager<cshader>);
		};

	} //- resource

} //- sm