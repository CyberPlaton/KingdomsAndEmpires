#pragma once
#include <core.h>
#include "../sm_config.hpp"
#include "sm_tiny_shading_language_lexer.hpp"

namespace sm
{
	namespace tsl
	{
		class itiny_shader_compiler;
		using tiny_shader_compiler_ref_t = ref_t<itiny_shader_compiler>;

		//------------------------------------------------------------------------------------------------------------------------
		enum compile_result : uint8_t
		{
			compile_result_ok = 0,
			compile_result_fail = 255,
		};

		//- Interface class responsible for compiling tiny shader language to target shading language.
		//------------------------------------------------------------------------------------------------------------------------
		class itiny_shader_compiler
		{
		public:
			virtual compile_result	compile(stringview_t) = 0;
			virtual compile_result	compile(const memory_ref_t&) = 0;
			virtual string_t		emit() = 0;
		};

	} //- tsl

} //- sm