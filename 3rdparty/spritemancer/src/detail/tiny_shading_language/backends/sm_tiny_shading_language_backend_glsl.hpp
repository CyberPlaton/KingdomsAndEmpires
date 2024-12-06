#pragma once
#include "../sm_tiny_shading_language_backend.hpp"

namespace sm
{
	namespace tsl
	{
		//- Interface class responsible for compiling tiny shader language OpenGL GLSL.
		//------------------------------------------------------------------------------------------------------------------------
		class ctiny_shader_compiler_glsl final : public itiny_shader_compiler
		{
		public:
			compile_result	compile(stringview_t source) override final;
			compile_result	compile(const memory_ref_t& source) override final;
			string_t		emit() override final;

		private:


		private:
			compile_result do_compile();
		};

	} //- tsl

} //- sm