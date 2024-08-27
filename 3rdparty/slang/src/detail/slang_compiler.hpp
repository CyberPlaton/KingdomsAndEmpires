#pragma once
#include "slang_types.hpp"
#include "slang_object.hpp"
#include "slang_chunk.hpp"
#include "slang_scanner.hpp"

namespace slang
{
	namespace detail
	{
		namespace expressions
		{
			void primary(detail::ccompiling_context& ctx);
			void call(detail::ccompiling_context& ctx);
			void unary(detail::ccompiling_context& ctx);
			void factor(detail::ccompiling_context& ctx);
			void term(detail::ccompiling_context& ctx);
			void comparison(detail::ccompiling_context& ctx);
			void equality(detail::ccompiling_context& ctx);
			void logacal_and(detail::ccompiling_context& ctx);
			void logical_or(detail::ccompiling_context& ctx);
			void assignment(detail::ccompiling_context& ctx);
			void expression(detail::ccompiling_context& ctx);

		} //- expressions

		namespace statements
		{
			void statement_expression(detail::ccompiling_context& ctx);
			void statement_for(detail::ccompiling_context& ctx);
			void statement_print(detail::ccompiling_context& ctx);
			void statement_return(detail::ccompiling_context& ctx);
			void statement_if(detail::ccompiling_context& ctx);
			void statement_while(detail::ccompiling_context& ctx);
			void block(detail::ccompiling_context& ctx);
			void statement(detail::ccompiling_context& ctx);

		} //- statements

		namespace rules
		{
			void declaration_function(detail::ccompiling_context& ctx);
			void declaration_variable(detail::ccompiling_context& ctx);
			void declaration_class(detail::ccompiling_context& ctx);
			void declaration(detail::ccompiling_context& ctx);

		} //- rules

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

} //- slang