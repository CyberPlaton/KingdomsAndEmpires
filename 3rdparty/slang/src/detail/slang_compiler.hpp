#pragma once
#include "slang_types.hpp"
#include "slang_object.hpp"

namespace slang
{
	namespace detail
	{
		//- Scanner and bytecode generator. Takes code text and creates an intermediate representation ready to be run on a VM
		//------------------------------------------------------------------------------------------------------------------------
		class ccompiler
		{
		public:
			ccompiler() = default;
			~ccompiler() = default;

			compile_result compile(stringview_t code);
			[[nodiscard]] inline detail::schunk take() const {return std::move(m_chunk);};

		private:
			struct scursor
			{
				string_t m_text;
				uint32_t m_current = 0;
				uint32_t m_line = 0;
			};

			detail::schunk m_chunk;
			scursor m_cursor;
			stringview_t m_code;
			compile_result m_result = compile_result_ok;
			
		private:
			detail::stoken next_token();
			void process_token(const detail::stoken& token);

			inline char peek(uint32_t lookahead = 0) const
			{
				SLANG_ASSERT(m_cursor.m_current + lookahead < m_code.size(), "Invalid operation. Index out of bound");
				return m_code[m_cursor.m_current + lookahead];
			}

			inline char advance() { return m_code[m_cursor.m_current++]; }
		};

	} //- detail

} //- slang