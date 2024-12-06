#include "sm_tiny_shading_language_backend_essl.hpp"

namespace sm
{
	namespace tsl
	{
		//------------------------------------------------------------------------------------------------------------------------
		compile_result ctiny_shader_compiler_essl::compile(stringview_t source)
		{
			return compile_result_fail;
		}

		//------------------------------------------------------------------------------------------------------------------------
		compile_result ctiny_shader_compiler_essl::compile(const memory_ref_t& source)
		{
			return compile_result_fail;
		}

		//------------------------------------------------------------------------------------------------------------------------
		string_t ctiny_shader_compiler_essl::emit()
		{
			return {};
		}

	} //- tsl

} //- sm