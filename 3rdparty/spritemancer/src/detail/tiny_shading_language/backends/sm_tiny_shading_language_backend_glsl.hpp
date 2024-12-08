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
			compile_result do_compile();

			void do_directive(const stoken& instruction, const stoken& string);

			void do_declaration_function();
			void do_declaration_structure();
			void do_declaration_variable();

			void do_function_parameters();
			void do_annotations_body();
			void do_function_body();
		};

	} //- tsl

} //- sm