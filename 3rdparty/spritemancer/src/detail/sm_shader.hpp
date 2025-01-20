#pragma once
#include "sm_config.hpp"
#include "sm_shader_compiler.hpp"

namespace sm
{
	namespace resource
	{
		namespace detail
		{
			//- Struct containing a single vertex, pixle or compute shader.
			//------------------------------------------------------------------------------------------------------------------------
			struct sshader final
			{
				string_t m_name;
				memory_ref_t m_blob;
				bgfx::ShaderHandle m_handle;
				shader_type m_type;
			};

		} //- detail

		//- Shader resource class. Extension of the shader file is constructed from whether it is compiled or a raw text file
		//- and whether it is a vertex, pixel or compute shader, e.g.
		//- Raw/compiled compute shader: '.cs' and '.ccs'
		//- Raw/compiled vertex shader: '.ps' and '.cps'
		//- Raw/compiled pixel shader: '.vs' and '.cvs'
		//------------------------------------------------------------------------------------------------------------------------
		class cshader final : public core::cresource<detail::sshader>
		{
		public:
			cshader() = default;
			~cshader();

		private:
			RTTR_ENABLE(core::cresource<detail::sshader>);
		};

		//- Shader resource manager class responsible for loading a shader from file or string.
		//------------------------------------------------------------------------------------------------------------------------
		class cshader_manager final : public core::cresource_manager<cshader>
		{
		public:
			cshader_manager() = default;
			~cshader_manager() = default;

		protected:
			const core::resource::iresource* load(stringview_t name, const fs::cfileinfo& path) override final;

		private:
			RTTR_ENABLE(core::cresource_manager<cshader>);
		};

	} //- resource

} //- sm