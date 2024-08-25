#pragma once
#include "slang_types.hpp"
#include "slang_object.hpp"
#include "slang_chunk.hpp"
#include "slang_scanner.hpp"

namespace slang
{
	namespace detail
	{
		//------------------------------------------------------------------------------------------------------------------------
		class ccompiling_context final :
			private cnon_copyable,
			private cnon_movable
		{
		public:
			struct scursor
			{
				stoken_stream m_stream;
				uint32_t m_current = 0;
			};

			struct sconfig
			{

			};

			explicit ccompiling_context(stoken_stream&& stream, sconfig cfg = {});
			~ccompiling_context() = default;

			inline cchunk& chunk() { return m_chunk; }
			inline scursor& cursor() { return m_cursor; }
			inline compile_result& result() { return m_result; }
			[[nodiscard]] cchunk&& take_chunk() { return std::move(m_chunk); }

		private:
			cchunk m_chunk;
			scursor m_cursor;
			compile_result m_result = compile_result_ok;
		};

	} //- detail

	//------------------------------------------------------------------------------------------------------------------------
	class ccompiler final
	{
	public:
		explicit ccompiler(stoken_stream&& stream, detail::ccompiling_context::sconfig cfg = {});
		~ccompiler() = default;

		bool compile();

		[[nodiscard]] cchunk&& chunk() { return std::move(m_ctx.take_chunk()); }

	private:
		detail::ccompiling_context m_ctx;
	};



	namespace detail
	{
		//- Responsible for generating an  intermediate representation ready to be run on a VM.
		//------------------------------------------------------------------------------------------------------------------------
		class ccompiler final
		{
		public:
			ccompiler() = default;
			~ccompiler() = default;

			compile_result compile(stringview_t code);
			[[nodiscard]] inline cchunk& chunk() { SLANG_ASSERT(m_result == compile_result_ok, "Invalid operation. Compilation was invalid"); return m_compiler.m_chunk;};

		private:
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
			SLANG_ASSERT(index < std::numeric_limits<byte_t>::max(), "Invalid operation. 'Constant' limit for chunk reached");

			return static_cast<byte_t>(index);
		}

	} //- detail

} //- slang