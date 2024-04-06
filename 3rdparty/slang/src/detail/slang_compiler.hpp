#pragma once
#include "slang_types.hpp"
#include "slang_object.hpp"

namespace slang
{
	namespace detail
	{
		//- Scanner and bytecode generator. Takes code text and creates an intermediate representation ready to be run on a VM.
		//- Scanner and compiler part are separated internally, but not visible to outside
		//------------------------------------------------------------------------------------------------------------------------
		class ccompiler
		{
		public:
			ccompiler() = default;
			~ccompiler() = default;

			compile_result compile(stringview_t code);
			[[nodiscard]] inline schunk& chunk() { SLANG_ASSERT(m_result == compile_result_ok, "Invalid operation. Compilation was invalid"); return m_chunk;};

		private:
			struct scursor
			{
				string_t m_text;
				uint32_t m_current = 0;
				uint32_t m_line = 0;
			};

			//- scanning part
			struct sscanner
			{
				scursor m_cursor;
				stoken_stream m_token_stream;
				stringview_t m_code;
				bool m_panik = false;
				compile_result m_result = compile_result_ok;

				compile_result compile(stringview_t code);
				stoken next_token();
				void process_token(stoken&& token);
				char peek(uint32_t lookahead = 0) const;
				inline char advance() { return m_code[++m_cursor.m_current]; }
				inline void panik() { m_panik = true; }
				inline const bool is_paniking() const { return m_panik; }

				bool is_identifier(char c) const;
				bool is_eof(char c) const;
				bool is_digit(char c) const;
				bool is_whitespace(char c) const;
				bool is_newline(char c) const;
				bool is_comment(char c) const;
				bool is_keyword(stringview_t text, token_type type) const;

				stoken make_identifier();
				stoken make_number();
				stoken make_string();
				stoken make_error(stringview_t text);
				[[nodiscard]] inline stoken make_token(uint32_t line, stringview_t text, token_type type) { return { line, text.data(), type }; }
			};

			//- compiling part
			struct scompiler
			{
				compile_result m_result = compile_result_ok;

				compile_result compile(stoken_stream& stream);
			};

			sscanner m_scanner;
			scompiler m_compiler;
			schunk m_chunk;
			compile_result m_result = compile_result_fail;

		private:
			void reset();

		};

	} //- detail

} //- slang