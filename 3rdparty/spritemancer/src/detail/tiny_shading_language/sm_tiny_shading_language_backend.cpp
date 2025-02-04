#include "sm_tiny_shading_language_backend.hpp"
#include "emitters/sm_tiny_shading_language_emitter_essl.hpp"
#include "emitters/sm_tiny_shading_language_emitter_glsl.hpp"

namespace sm
{
	namespace tsl
	{
		namespace detail
		{
			//------------------------------------------------------------------------------------------------------------------------
			ccompiling_context::ccompiling_context() :
				m_result(true), m_cfg(compiling::sconfig{})
			{
			}

			//------------------------------------------------------------------------------------------------------------------------
			void ccompiling_context::init(token_stream_t&& stream, compiling::sconfig cfg /*= {}*/)
			{
				m_stream = std::move(stream);
				m_cfg = cfg;
			}

			//------------------------------------------------------------------------------------------------------------------------
			stoken ccompiling_context::consume(token_type expect, const string_t& message)
			{
				if (peek().m_type == expect)
				{
					return advance();
				}

				emit_error(message);

				return { message, cursor().m_line, token_type_error };
			}

			//------------------------------------------------------------------------------------------------------------------------
			stoken ccompiling_context::consume(token_type expect_range_start, token_type expect_range_end, const string_t& message)
			{
				if (const auto& type = peek().m_type; type >= expect_range_start && type <= expect_range_end)
				{
					return advance();
				}

				emit_error(message);

				return { message, cursor().m_line, token_type_error };
			}

			//------------------------------------------------------------------------------------------------------------------------
			stoken ccompiling_context::advance()
			{
				const auto& out = m_stream[++m_cursor.m_current];
				m_cursor.m_line = out.m_line;

				return out;
			}

			//------------------------------------------------------------------------------------------------------------------------
			void ccompiling_context::begin_scope_vertex_shader()
			{
				scope(shader_scope_vertex);
			}

			//------------------------------------------------------------------------------------------------------------------------
			void ccompiling_context::begin_scope_pixel_shader()
			{
				scope(shader_scope_pixel);
			}

			//------------------------------------------------------------------------------------------------------------------------
			void ccompiling_context::begin_scope_global()
			{
				scope(shader_scope_global);
			}

			//------------------------------------------------------------------------------------------------------------------------
			void ccompiling_context::end_scope()
			{
				begin_scope_global();
			}

			//------------------------------------------------------------------------------------------------------------------------
			ccompiling_context& ccompiling_context::write(const string_t& text)
			{
				switch (scope())
				{
				default:
				case shader_scope_global: { return write_global(text); }
				case shader_scope_vertex: { return write_vertex_shader(text); }
				case shader_scope_pixel: { return write_pixel_shader(text); }
				}
			}

			//------------------------------------------------------------------------------------------------------------------------
			std::pair<string_t, string_t> ccompiling_context::emit_shader()
			{
				return std::make_pair(emit_vertex_shader(), emit_pixel_shader());
			}

			//------------------------------------------------------------------------------------------------------------------------
			string_t ccompiling_context::emit_vertex_shader()
			{
				return m_data.m_output_vertex_shader.string();
			}

			//------------------------------------------------------------------------------------------------------------------------
			string_t ccompiling_context::emit_pixel_shader()
			{
				return m_data.m_output_pixel_shader.string();
			}

			//------------------------------------------------------------------------------------------------------------------------
			void ccompiling_context::emit_error(const string_t& message)
			{
				result() = false;

				log_error(fmt::format("Compilation error '{}' at line '{}'!",
					message, cursor().m_line));
			}

			//------------------------------------------------------------------------------------------------------------------------
			bool ccompiling_context::is_data_type_token(const stoken& token)
			{
				return token.m_type >= token_type_void && token.m_type <= token_type_sampler2D;
			}

			//------------------------------------------------------------------------------------------------------------------------
			ccompiling_context& ccompiling_context::write_global(const string_t& text)
			{
				write_vertex_shader(text);
				write_pixel_shader(text);
				return *this;
			}

			//------------------------------------------------------------------------------------------------------------------------
			ccompiling_context& ccompiling_context::write_vertex_shader(const string_t& text)
			{
				m_data.m_output_vertex_shader.write(text);
				return *this;
			}

			//------------------------------------------------------------------------------------------------------------------------
			ccompiling_context& ccompiling_context::write_pixel_shader(const string_t& text)
			{
				m_data.m_output_pixel_shader.write(text);
				return *this;
			}

		} //- detail

		//------------------------------------------------------------------------------------------------------------------------
		ctiny_shader_compiler::ctiny_shader_compiler(shader_language language)
		{
			switch (language)
			{
			case shader_language_glsl:
			{
				m_emitter = std::make_shared<ctiny_shader_emitter_glsl>();
				break;
			}
			case shader_language_essl:
			{
				m_emitter = std::make_shared<ctiny_shader_emitter_essl>();
				break;
			}
			default:
			case shader_language_none:
			{
				CORE_ASSERT(false, "Invalid operation. No or unsupported shader language provided!");
				break;
			}
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		compile_result ctiny_shader_compiler::compile(const memory_ref_t& source)
		{
			return compile(stringview_t(source->data(), source->size()));
		}

		//------------------------------------------------------------------------------------------------------------------------
		compile_result ctiny_shader_compiler::compile(stringview_t source)
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
		string_t ctiny_shader_compiler::emit()
		{
			return {};
		}

		//------------------------------------------------------------------------------------------------------------------------
		compile_result ctiny_shader_compiler::do_compile()
		{
			do_program();

			return m_ctx.result() ? compile_result_ok : compile_result_fail;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ctiny_shader_compiler::do_program()
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
				else if (token.m_type == token_type_at)
				{
					const auto next_type = m_ctx.peek(1).m_type;

					//- Check whether we want to begin new scope or whether we open/close an annotation body
					if (next_type == token_type_VERTEX ||
						next_type == token_type_PIXEL)
					{
						do_shader_scope();
					}
					else
					{
						do_annotations_body();
					}
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
					m_ctx.emit_error(fmt::format("Uncrecognized token type '{}' with text '{}'!",
						algorithm::enum_to_string(token.m_type), token.m_text));
					break;
				}
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ctiny_shader_compiler::do_directive(const stoken& instruction, const stoken& string)
		{
			if (string_utils::compare(instruction.m_text, "include"))
			{
				m_ctx.consume(token_type_string, "Expected an include file path for '#include' directive!");

				log_debug(fmt::format("Processing include '{}'", string.m_text.data()));
			}
			else if (string_utils::compare(instruction.m_text, "pragma"))
			{
				m_ctx.consume(token_type_string, "Expected a supported pragma instruction for '#pragma' directive!");

				log_debug(fmt::format("Processing pragma '{}'", string.m_text.data()));
			}
		}

		//- <declaration_function> := "fn" <identifier> "(" <function_parameters> ")" "->" <type>
		//- 						("@" <annotations_body> "@")?
		//- 						"{" <function_body> "}" ";";
		//------------------------------------------------------------------------------------------------------------------------
		void ctiny_shader_compiler::do_declaration_function()
		{
			const auto identifier_token = m_ctx.consume(token_type_fn, "Missing function name after 'fn' for function declaration!");
			m_ctx.consume(token_type_identifier, "");

			//- Name of the function
			if (const auto function_name = m_emitter->emit_function_name(identifier_token.m_text); function_name.empty())
			{
				log_error(fmt::format("Emitter '{}' failed to emit function '{}'", m_emitter->emitter_name(), identifier_token.m_text.data()));
			}

			//- Emit the function return type and function name
			//- FIXME: to get the return type we have to lookahead and retrieve the type set after '->', and we have to do it here
			//- because we start potentially writing for function parameters...
			// const auto func_return_type = m_emitter->emit_data_type(...);

			m_ctx.consume(token_type_left_paren, "Opening '(' expected for function declaration!");

			//- Optionally, process function parameters
			{
				do_function_parameters();
			}

			m_ctx.consume(token_type_right_paren, "Closing ')' expected for function declaration!");

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
			m_ctx.consume(token_type_left_brace, "Opening '{' expected for function body in function declaration!");

			do_function_body();

			m_ctx.consume(token_type_right_brace, "Closing '}' expected for function body in function declaration!");
			m_ctx.consume(token_type_semicolon, "Closing ';' expected after function declaration!");
		}

		//- <declaration_structure> := "struct" <identifier> "{" <structure_body> "}" ";";
		//------------------------------------------------------------------------------------------------------------------------
		void ctiny_shader_compiler::do_declaration_structure()
		{

		}

		//- <declaration_variable> := <type> <identifier> "=" <expression> ";";
		//------------------------------------------------------------------------------------------------------------------------
		void ctiny_shader_compiler::do_declaration_variable()
		{

		}

		//- <shader_scope> := "@" <shader_type> <shader_body> "@";
		//------------------------------------------------------------------------------------------------------------------------
		void ctiny_shader_compiler::do_shader_scope()
		{
			const auto& scope_token = m_ctx.consume(token_type_at, "");

			//- Make sure we can open a new scope
			if (m_ctx.scope() != detail::ccompiling_context::shader_scope_global)
			{
				m_ctx.emit_error(fmt::format("Trying to open scope '{}' inside of scope '{}' is illegal! Pop current scope before opening a new one.",
					algorithm::enum_to_string(scope_token.m_type),
					algorithm::enum_to_string(m_ctx.scope())));
				return;
			}
			else
			{
				auto scope_value = detail::ccompiling_context::shader_scope_global;

				if (string_utils::compare(scope_token.m_text, "VERTEX"))
				{
					scope_value = detail::ccompiling_context::shader_scope_vertex;
				}
				else if (string_utils::compare(scope_token.m_text, "PIXEL"))
				{
					scope_value = detail::ccompiling_context::shader_scope_pixel;
				}
				else
				{
					m_ctx.emit_error(fmt::format("Trying to open unrecognized scope '{}'! Supported values are 'VERTEX' and 'PIXEL'",
						scope_token.m_text));
					return;
				}

				m_ctx.scope(scope_value);


			}
		}

		//- <shader_body> := ( <directive>
		//-					| <declaration_function>
		//-					| <declaration_structure>
		//-					| <declaration_variable>)*;
		//------------------------------------------------------------------------------------------------------------------------
		void ctiny_shader_compiler::do_shader_body()
		{

		}

		//- <function_parameters> := ( <function_parameter> ("," <function_parameter>)* )*;
		//-
		//- <function_parameter> := <identifier> ":" <type>;
		//------------------------------------------------------------------------------------------------------------------------
		void ctiny_shader_compiler::do_function_parameters()
		{
			bool first = true;

			for (;;)
			{
				//- Get the type and identifier tokens
				stoken identifier_token;

				if (first)
				{
					identifier_token = m_ctx.peek();
				}
				else
				{
					//- Consume type token and comma
					m_ctx.consume(token_type_void, token_type_sampler2D, "");
					identifier_token = m_ctx.consume(token_type_comma, "");
				}

				const auto colon_token = m_ctx.consume(token_type_identifier, "");
				const auto type_token = m_ctx.consume(token_type_colon, "");

				const auto parameter_type_name = m_emitter->emit_data_type(type_token.m_type);

				m_ctx.write(fmt::format("{}{} {}",
					first ? "" : ", ",
					parameter_type_name,
					identifier_token.m_text));

				first = false;

				//- Proceed until we reached end
				if (m_ctx.peek(1).m_type == token_type_right_paren)
				{
					m_ctx.advance(); break;
				}
			}
		}

		//- <annotations_body> := ( <declaration_annotation_member> )*;
		//-
		//- <declaration_annotation_member> := <type> <identifier> "=" <identifier> ";";
		//------------------------------------------------------------------------------------------------------------------------
		void ctiny_shader_compiler::do_annotations_body()
		{

		}

		//- <function_body> := (<statement>)*;
		//------------------------------------------------------------------------------------------------------------------------
		void ctiny_shader_compiler::do_function_body()
		{

		}

	} //- tsl

} //- sm