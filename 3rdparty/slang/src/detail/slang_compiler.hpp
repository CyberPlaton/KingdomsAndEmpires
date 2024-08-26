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
				unsigned m_current = 0;
			};

			struct sconfig
			{

			};

			explicit ccompiling_context(stoken_stream&& stream, sconfig cfg = {});
			~ccompiling_context() = default;

			inline cchunk& chunk() { return m_chunk; }
			inline scursor& cursor() { return m_cursor; }
			inline const scursor& cursor() const { return m_cursor; }
			inline sconfig& cfg() { return m_cfg; }
			inline compile_result& result() { return m_result; }
			inline stoken_stream& stream() { return cursor().m_stream; }
			inline vector_t<stoken>& tokens() { return cursor().m_stream.m_tokens; }
			inline const vector_t<stoken>& tokens() const { return cursor().m_stream.m_tokens; }
			[[nodiscard]] cchunk&& take_chunk() { return std::move(m_chunk); }

			void process_token();
			stoken& advance();
			const stoken& peek(unsigned lookahead = 0) const;
			bool consume(token_type expected);
			unsigned current_line() const;

			bool match_token_type(token_type type);
			bool check_token_type(token_type type) const;

		private:
			cchunk m_chunk;
			scursor m_cursor;
			sconfig m_cfg;
			compile_result m_result = compile_result_ok;
		};

	} //- detail

	//- Responsible for 
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
			};
		};

	} //- detail

} //- slang