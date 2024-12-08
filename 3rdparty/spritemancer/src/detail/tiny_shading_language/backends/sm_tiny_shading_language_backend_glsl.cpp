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
			ctiny_shader_language_lexer lexer(source.data());

			if (!lexer.scan())
			{
				return compile_result_fail;
			}

			compiling::sconfig cfg;

			m_ctx.init(std::move(lexer.stream()), cfg);

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
			for (;;)
			{
				const auto& token = m_ctx.peek();

				if (token.m_type == token_type_directive)
				{
					//- Read the next identifier as an instruction
					const auto& inst = m_ctx.consume(token_type_directive, "");
					const auto& text = m_ctx.consume(token_type_identifier, "");

					do_directive(inst, text);
				}
				else if (token.m_type == token_type_fn)
				{
					do_declaration_function();
				}
				else if (token.m_type == token_type_struct)
				{
					do_declaration_structure();
				}
				else if (m_ctx.is_data_type_token(token))
				{
					do_declaration_variable();
				}
				else
				{
					log_error(fmt::format("Uncrecognized token type '{}' with text '{}' at line '{}'!",
						algorithm::enum_to_string(token.m_type), token.m_text, token.m_line));

					m_ctx.result() = false;
				}
			}
			return compile_result_fail;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ctiny_shader_compiler_glsl::do_directive(const stoken& instruction, const stoken& string)
		{
			if (string_utils::compare(instruction.m_text, "include"))
			{
				m_ctx.consume(token_type_string, "Expected an include file path for '#include' directive!");
			}
			else if (string_utils::compare(instruction.m_text, "pragma"))
			{
				m_ctx.consume(token_type_string, "Expected a supported pragma instruction for '#pragma' directive!");
			}
		}

		//- <declaration_function> := "fn" <identifier> "(" <function_parameters> ")" "->" <type>
		//- 						("@" <annotations_body> "@")?
		//- 						"{" <function_body> "}" ";";
		//------------------------------------------------------------------------------------------------------------------------
		void ctiny_shader_compiler_glsl::do_declaration_function()
		{
			m_ctx.consume(token_type_fn, "Function declaration must start with 'fn' keyword!");
			const auto identifier_token = m_ctx.consume(token_type_identifier, "Missing function name after 'fn' for function declaration!");

			//- Name of the function
			const auto& func_name = identifier_token.m_text;

			m_ctx.consume(token_type_right_paren, "Opening '(' expected for function declaration!");

			//- Optionally, process function parameters
			{
				do_function_parameters();
			}

			m_ctx.consume(token_type_left_paren, "Closing ')' expected for function declaration!");

			//- Expect the return type of the function and extract it
			m_ctx.consume(token_type_minus, "Function return type assignment '->' expected for function declaration!");
			m_ctx.consume(token_type_greater, "Function return type assignment '->' expected for function declaration!");

			const auto& type_token = m_ctx.consume(token_type_void, token_type_sampler2D, "Function return type is not valid for function declaration!");

			//- Optionally, process annotations body
			if (m_ctx.peek().m_type == token_type_at)
			{
				do_annotations_body();

				m_ctx.consume(token_type_at, "Closing '@' expected for annotations body in function declaration!");
			}

			//- Process function body (Note: we do not support forward declarations)
			m_ctx.consume(token_type_right_brace, "Opening '{' expected for function body in function declaration!");

			do_function_body();

			m_ctx.consume(token_type_left_brace, "Closing '}' expected for function body in function declaration!");
			m_ctx.consume(token_type_semicolon, "Closing ';' expected after function declaration!");
		}

		//- <declaration_structure> := "struct" <identifier> "{" <structure_body> "}" ";";
		//------------------------------------------------------------------------------------------------------------------------
		void ctiny_shader_compiler_glsl::do_declaration_structure()
		{

		}

		//- <declaration_variable> := <type> <identifier> "=" <expression> ";";
		//------------------------------------------------------------------------------------------------------------------------
		void ctiny_shader_compiler_glsl::do_declaration_variable()
		{

		}

		//- <function_parameters> := ( <function_parameter> ("," <function_parameter>)* )*;
		//-
		//- <function_parameter> := <identifier> ":" <type>;
		//------------------------------------------------------------------------------------------------------------------------
		void ctiny_shader_compiler_glsl::do_function_parameters()
		{

		}

		//- <annotations_body> := ( <declaration_annotation_member> )*;
		//-
		//- <declaration_annotation_member> := <type> <identifier> "=" <identifier> ";";
		//------------------------------------------------------------------------------------------------------------------------
		void ctiny_shader_compiler_glsl::do_annotations_body()
		{

		}

		//- <function_body> := (<statement>)*;
		//------------------------------------------------------------------------------------------------------------------------
		void ctiny_shader_compiler_glsl::do_function_body()
		{

		}

	} //- tsl

} //- sm