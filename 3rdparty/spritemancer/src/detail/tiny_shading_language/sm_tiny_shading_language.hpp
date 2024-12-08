#pragma once
#include <core.h>
#include "../sm_config.hpp"
#include "sm_tiny_shading_language_backend.hpp"

namespace sm
{
	namespace tsl
	{
		//- Public interface class responsible for creating the appropriate compiler for selected language and
		//- provides functions to compile and emit target shader code.
		//------------------------------------------------------------------------------------------------------------------------
		class ctiny_shader_language final
		{
		public:
			ctiny_shader_language(shader_language language);
			~ctiny_shader_language() = default;

			compile_result	compile(stringview_t source);
			compile_result	compile(const memory_ref_t& source);
			string_t		emit();

		private:
			ctiny_shader_compiler m_compiler;
			shader_language m_language;
		};

	} //- tsl

} //- sm