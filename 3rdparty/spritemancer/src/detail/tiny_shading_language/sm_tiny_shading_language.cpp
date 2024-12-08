#include "sm_tiny_shading_language.hpp"
#include "backends/sm_tiny_shading_language_backend_essl.hpp"
#include "backends/sm_tiny_shading_language_backend_glsl.hpp"

namespace sm
{
	namespace tsl
	{
		//------------------------------------------------------------------------------------------------------------------------
		ctiny_shader_language::ctiny_shader_language(shader_language language) :
			m_compiler(language)
		{
		}

		//------------------------------------------------------------------------------------------------------------------------
		compile_result ctiny_shader_language::compile(stringview_t source)
		{
			return m_compiler.compile(source);
		}

		//------------------------------------------------------------------------------------------------------------------------
		compile_result ctiny_shader_language::compile(const memory_ref_t& source)
		{
			return m_compiler.compile(source);
		}

		//------------------------------------------------------------------------------------------------------------------------
		string_t ctiny_shader_language::emit()
		{
			return m_compiler.emit();
		}

	} //- tsl

} //- sm