#pragma once
#include "detail/slang_types.hpp"
#include "detail/slang_os_api.hpp"
#include "detail/slang_debug.hpp"

namespace slang
{
	//- Holds allocated constant values. They live throughout a VM lifetime and do not change.
	//- i.e. var value = 1; here '1' is the constant.
	//------------------------------------------------------------------------------------------------------------------------
	struct sconstants
	{
		vector_t<svalue> m_values;
	};

	//- Holds allocated values that are allocated dynamically and can change their values
	//------------------------------------------------------------------------------------------------------------------------
	struct svariables
	{
		umap_t<uint64_t, svalue> m_values;
	};

	//- Data wrapper
	//------------------------------------------------------------------------------------------------------------------------
	struct scompiled_data
	{
		svariables m_variables;
		sconstants m_constants;
	};

	//------------------------------------------------------------------------------------------------------------------------
	class ccompiler
	{
	public:
		ccompiler();
		~ccompiler() = default;

		compile_result compile(stringview_t code, scompiled_data& data_out, detail::schunk& code_out);

	private:
		struct scursor
		{
			string_t m_text;
			uint32_t m_index = 0;
			uint32_t m_line = 0;
		};

		scompiled_data m_data;
		scursor m_cursor;
		detail::schunk m_bytecode;
		stringview_t m_code;
		compile_result m_result = compile_result_ok;

	private:
		detail::stoken next_token();
		void process_token(const detail::stoken& token);

		inline char peek(uint32_t lookahead = 0) const
		{
			SLANG_ASSERT(m_cursor.m_index + lookahead < m_code.size(), "Invalid operation. Index out of bound");
			return m_code[m_cursor.m_index + lookahead];
		}

		inline char advance() { return m_code[m_cursor.m_index++]; }
	};

} //- slang