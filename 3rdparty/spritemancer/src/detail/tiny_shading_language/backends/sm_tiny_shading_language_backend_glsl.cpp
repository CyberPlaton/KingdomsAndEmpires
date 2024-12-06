#include "sm_tiny_shading_language_backend_glsl.hpp"

namespace sm
{
	namespace tsl
	{
		namespace
		{


		} //- unnamed

		//------------------------------------------------------------------------------------------------------------------------
		compile_result ctiny_shader_compiler_glsl::compile(const memory_ref_t& source)
		{
			return compile(stringview_t(source->data(), source->size()));
		}

		//------------------------------------------------------------------------------------------------------------------------
		compile_result ctiny_shader_compiler_glsl::compile(stringview_t source)
		{
			sm::tsl::ctiny_shader_language_lexer lexer(source.data());

			if (!lexer.scan())
			{
				return compile_result_fail;
			}

			m_stream = std::move(lexer.stream());

			return do_compile();
		}

		//------------------------------------------------------------------------------------------------------------------------
		string_t ctiny_shader_compiler_glsl::emit()
		{
			return {};
		}

		//------------------------------------------------------------------------------------------------------------------------
		compile_result ctiny_shader_compiler_glsl::do_compile()
		{

		}

	} //- tsl

} //- sm