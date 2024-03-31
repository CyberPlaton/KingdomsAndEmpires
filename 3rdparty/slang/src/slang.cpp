#include "slang.hpp"
#include "detail/slang_types.hpp"

namespace slang
{
	namespace
	{
		inline static uint64_t s_variable_identifier = 0;

		//------------------------------------------------------------------------------------------------------------------------
		inline static void emit_byte(detail::schunk& code, byte_t byte)
		{
			code.m_code.emplace_back(byte);
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline static void emit_bytes(detail::schunk& code, byte_t byte1, byte_t byte2)
		{
			emit_byte(code, byte1);
			emit_byte(code, byte2);
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	ccompiler::ccompiler()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	compile_result ccompiler::compile(stringview_t code, scompiled_data& data_out, detail::schunk& code_out)
	{
		m_code = code;

		for (auto i = 0; i < m_code.size(); ++i)
		{
			const auto& c = m_code[i];

			process_token(next_token());
		}

		return m_result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	detail::stoken ccompiler::next_token()
	{
		m_cursor.m_text.clear();

		auto c = peek();


	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccompiler::process_token(const detail::stoken& token)
	{

	}

} //- slang