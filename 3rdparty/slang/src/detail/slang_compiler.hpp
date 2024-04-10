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
			[[nodiscard]] inline schunk& chunk() { SLANG_ASSERT(m_result == compile_result_ok, "Invalid operation. Compilation was invalid"); return m_compiler.m_chunk;};

		private:
			//- scanning part
			struct sscanner
			{
				struct scursor
				{
					string_t m_text;
					uint32_t m_current = 0;
					uint32_t m_line = 0;
				};

				scursor m_cursor;
				stoken_stream m_token_stream;
				stringview_t m_code;
				compile_result m_result = compile_result_ok;

				compile_result scan(stringview_t code);
				stoken next_token();
				void process_token(stoken&& token);
				char peek(uint32_t lookahead = 0) const;
				inline char advance() { return m_code[++m_cursor.m_current]; }

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
				using parse_function_t = void(scompiler::*)();

				struct scursor
				{
					stoken_stream m_stream;
					uint32_t m_current = 0;
				};

				struct sparse_rule
				{
					sparse_rule(parse_function_t prefix, parse_function_t infix, precedence_type prec) :
						m_prefix(prefix), m_infix(infix), m_precedence(prec)
					{}

					parse_function_t m_prefix = nullptr;
					parse_function_t m_infix = nullptr;
					precedence_type m_precedence = precedence_type_none;
				};

				schunk m_chunk;
				scursor m_cursor;
				compile_result m_result = compile_result_ok;

				compile_result compile(stoken_stream&& stream);
				void process_current_token();
				inline stoken& advance() {return m_cursor.m_stream.m_stream[++m_cursor.m_current];}
				stoken& peek(uint32_t lookahead = 0);
				void consume(token_type expected, stringview_t error_message);

				void emit_byte(byte_t byte, uint32_t line);

				void emit_error_at_current_token();
				void emit_error(uint32_t line, stringview_t message);

				//- Production rules making opcodes and data from tokens
				void parse_precedence(precedence_type type);
				void grouping();
				void call();
				void dot();
				void unary();
				void binary();
				void variable();
				void string();
				void number();
				void literal_();
				void this_();

				sparse_rule parse_rule(token_type type);

				void expression();

				template<typename T>
				void emit_constant(T&& value, uint32_t line);

				template<typename T>
				byte_t make_constant(T&& value);
			};

			sscanner m_scanner;
			scompiler m_compiler;
			compile_result m_result = compile_result_fail;

		private:
			void reset();
		};

		//------------------------------------------------------------------------------------------------------------------------
		template<typename T>
		void ccompiler::scompiler::emit_constant(T&& value, uint32_t line)
		{
			emit_byte(opcode_constant, line);
			emit_byte(make_constant<T>(std::move(value)), line);
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename T>
		byte_t ccompiler::scompiler::make_constant(T&& value)
		{
			auto index = m_chunk.m_constants.size();

			m_chunk.m_constants.emplace_back(svalue::create(std::move(value), value_type_number));

			//- TODO: in case this becomes a problem, we need to find a way to encode an index
			//- in 2 bytes or something similar
			SLANG_ASSERT(index < std::numeric_limits<byte_t>::max(), "Invalid operation. Constant limit for chunk reached");

			return static_cast<byte_t>(index);
		}

	} //- detail

} //- slang