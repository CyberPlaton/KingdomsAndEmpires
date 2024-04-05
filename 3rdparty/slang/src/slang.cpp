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
	cslang_state::cslang_state() :
		m_compiler(std::move(std::make_unique<detail::ccompiler>())),
		m_vm(std::move(std::make_unique<detail::cvm>()))
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	cslang_state::~cslang_state()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	compile_result cslang_state::compile(stringview_t code)
	{
		return m_compiler->compile(code);
	}

	//------------------------------------------------------------------------------------------------------------------------
	interpret_result cslang_state::run()
	{
		//- take chunk pointer/reference from compiler

		//- let VM execute chunk

		return interpret_result_fail;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cslang_state* slang_open()
	{
		return new cslang_state();
	}

	//------------------------------------------------------------------------------------------------------------------------
	slang::compile_result slang_compile(cslang_state* state, stringview_t code)
	{
		return state->compile(code);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void slang_close(cslang_state* state)
	{
		delete state;
	}

} //- slang