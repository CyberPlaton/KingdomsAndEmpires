#include "sm_tiny_shading_language_backend_glsl.hpp"

namespace sm
{
	namespace tsl
	{
		//------------------------------------------------------------------------------------------------------------------------
		compile_result ctiny_shader_compiler_glsl::compile(stringview_t source)
		{
			return compile_result_fail;
		}

		//------------------------------------------------------------------------------------------------------------------------
		compile_result ctiny_shader_compiler_glsl::compile(const memory_ref_t& source)
		{
			return compile_result_fail;
		}

		//------------------------------------------------------------------------------------------------------------------------
		string_t ctiny_shader_compiler_glsl::emit()
		{
			return {};
		}

	} //- tsl

} //- sm