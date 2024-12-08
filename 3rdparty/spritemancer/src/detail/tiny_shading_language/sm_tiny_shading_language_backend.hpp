#pragma once
#include <core.h>
#include "../sm_config.hpp"
#include "sm_tiny_shading_language_lexer.hpp"

namespace sm
{
	namespace tsl
	{
		class itiny_shader_emitter;
		using tiny_shader_emitter_ref_t = ref_t<itiny_shader_emitter>;

		//------------------------------------------------------------------------------------------------------------------------
		enum compile_result : uint8_t
		{
			compile_result_ok = 0,
			compile_result_fail = 255,
		};

		//- Interface class responsible for translating words to shader specific words as well as emitting functions, structs etc.
		//- in a way the target language requires.
		//------------------------------------------------------------------------------------------------------------------------
		class itiny_shader_emitter
		{
		public:
			//- Given a function name, translate it to target shader language name, if empty
			//- the compiler assumes the function is not a built-in one but user defined
			virtual string_t emit_function_name(const string_t&) = 0;

			//- Translate data type to specific target shader language name
			virtual string_t emit_data_type(token_type) = 0;
		};

		namespace compiling
		{
			//- Configuration of token compilation.
			//------------------------------------------------------------------------------------------------------------------------
			struct sconfig final
			{

			};

		} //- compiling

		namespace detail
		{
			//------------------------------------------------------------------------------------------------------------------------
			class ccompiling_context final :
				private core::cnon_copyable,
				private core::cnon_movable
			{
			public:
				enum shader_scope : uint8_t
				{
					shader_scope_none = 0,
					shader_scope_vertex,
					shader_scope_pixel,
					shader_scope_global
				};

				struct sdata
				{
					core::cstring_buffer m_output_vertex_shader;
					core::cstring_buffer m_output_pixel_shader;
					shader_scope m_current_scope = shader_scope_global;
				};

				struct scursor
				{
					stoken m_token;
					unsigned m_current	= 0;
					unsigned m_line		= 1;
				};

				struct smeta_data
				{
					vector_t<string_t> m_includes;
				};

				ccompiling_context();
				~ccompiling_context() = default;

				void init(token_stream_t&& stream, compiling::sconfig cfg = {});

				inline compiling::sconfig& cfg() { return m_cfg; }

				stoken consume(token_type expect, const string_t& message);
				stoken consume(token_type expect_range_start, token_type expect_range_end, const string_t& message);
				stoken advance();
				inline const stoken& peek(unsigned lookahead = 0) const { return m_stream[m_cursor.m_current + lookahead]; }
				inline bool at_end() const { return peek().m_type == token_type_eof; }

				inline bool& result() { return m_result; }
				inline smeta_data& meta() { return m_meta; }
				inline sdata& data() { return m_data; }
				inline scursor& cursor() { return m_cursor; }

				void begin_scope_vertex_shader();
				void begin_scope_pixel_shader();
				void begin_scope_global();
				void end_scope();
				ccompiling_context& write(const string_t& text);
				shader_scope scope() const { return m_data.m_current_scope; };
				void scope(shader_scope value) { m_data.m_current_scope = value; }

				std::pair<string_t, string_t> emit_shader();
				string_t emit_vertex_shader();
				string_t emit_pixel_shader();

				void emit_error(const string_t& message);
				bool is_data_type_token(const stoken& token);

			private:
				sdata m_data;
				smeta_data m_meta;
				scursor m_cursor;
				token_stream_t m_stream;
				compiling::sconfig m_cfg;
				bool m_result;

			private:
				ccompiling_context& write_global(const string_t& text);
				ccompiling_context& write_vertex_shader(const string_t& text);
				ccompiling_context& write_pixel_shader(const string_t& text);
			};

		} //- detail

		//- Class responsible for parsing a token stream and transpiling to requested shader language, i.e. GLSL or ESSL etc.
		//------------------------------------------------------------------------------------------------------------------------
		class ctiny_shader_compiler final
		{
		public:
			explicit ctiny_shader_compiler(shader_language language);
			~ctiny_shader_compiler() = default;

			compile_result	compile(stringview_t source);
			compile_result	compile(const memory_ref_t& source);
			string_t		emit();

		private:
			compile_result do_compile();

			void do_program();

			void do_directive(const stoken& instruction, const stoken& string);
			void do_declaration_function();
			void do_declaration_structure();
			void do_declaration_variable();

			void do_shader_scope();
			void do_shader_body();

			void do_function_parameters();
			void do_annotations_body();
			void do_function_body();

		private:
			detail::ccompiling_context m_ctx;
			string_t m_code;
			tiny_shader_emitter_ref_t m_emitter;
		};

	} //- tsl

} //- sm