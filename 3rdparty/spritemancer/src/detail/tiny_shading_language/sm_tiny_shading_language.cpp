#include "sm_tiny_shading_language.hpp"
#include "backends/sm_tiny_shading_language_backend_essl.hpp"
#include "backends/sm_tiny_shading_language_backend_glsl.hpp"

namespace sm
{
	namespace tsl
	{
		//------------------------------------------------------------------------------------------------------------------------
		ctiny_shader_language::ctiny_shader_language(shader_language language)
		{
			switch (language)
			{
			default:
			{
				log_error(fmt::format("Unsupported shader language: '{}'", algorithm::enum_to_string(language)));
				break;
			}
			case shader_language_glsl:
			{
				m_compiler = std::make_shared<ctiny_shader_compiler_glsl>();
				break;
			}
			case shader_language_essl:
			{
				m_compiler = std::make_shared<ctiny_shader_compiler_essl>();
				break;
			}
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		compile_result ctiny_shader_language::compile(stringview_t source)
		{
			CORE_ASSERT(m_compiler, "Invalid operation. A compiler was not set!");

			return m_compiler->compile(source);
		}

		//------------------------------------------------------------------------------------------------------------------------
		compile_result ctiny_shader_language::compile(const memory_ref_t& source)
		{
			CORE_ASSERT(m_compiler, "Invalid operation. A compiler was not set!");

			return m_compiler->compile(source);
		}

		//------------------------------------------------------------------------------------------------------------------------
		string_t ctiny_shader_language::emit()
		{
			CORE_ASSERT(m_compiler, "Invalid operation. A compiler was not set!");

			return m_compiler->emit();
		}

	} //- tsl

} //- sm