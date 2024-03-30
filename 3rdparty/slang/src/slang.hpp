#pragma once
#include "detail/slang_os_api.hpp"

namespace slang
{
	//- Result of compilation from source code to intermediate representation
	//------------------------------------------------------------------------------------------------------------------------
	enum compile_result : uint8_t
	{
		compile_result_ok = 0,
		compile_result_fail = 255,
	};

	//- Operations to be executed
	//------------------------------------------------------------------------------------------------------------------------
	enum opcode : uint32_t
	{
		opcode_noop = 0,
		opcode_add,
		opcode_subtract,
		opcode_multiply,
		opcode_divide,

		opcode_constant,
		opcode_variable,
	};

	//- Holds allocated constant values. They live throughout a VM lifetime and do not change
	//------------------------------------------------------------------------------------------------------------------------
	struct sconstants
	{
		vector_t<sinteger,	sos_api::sallocator> m_integers;
		vector_t<sfloat,	sos_api::sallocator> m_floats;
		vector_t<sboolean,	sos_api::sallocator> m_booleans;
		vector_t<sstring,	sos_api::sallocator> m_strings;
	};

	//- Holds allocated values that are allocated dynamically and can change their values
	//------------------------------------------------------------------------------------------------------------------------
	struct svariables
	{
		umap_t<uint64_t, snull,		sos_api::sallocator> m_nulls;
		umap_t<uint64_t, sinteger,	sos_api::sallocator> m_integers;
		umap_t<uint64_t, sfloat,	sos_api::sallocator> m_floats;
		umap_t<uint64_t, sboolean,	sos_api::sallocator> m_booleans;
		umap_t<uint64_t, sstring,	sos_api::sallocator> m_strings;
	};

	//- Data wrapper
	//------------------------------------------------------------------------------------------------------------------------
	struct scompiled_data
	{
		svariables m_variables;
		sconstants m_constants;
	};

	//- Holds compiled instructions
	//------------------------------------------------------------------------------------------------------------------------
	struct sbytecode
	{
		vector_t<opcode, sos_api::sallocator> m_code;
	};

	//------------------------------------------------------------------------------------------------------------------------
	class ccompiler
	{
	public:
		ccompiler() = default;
		~ccompiler() = default;

		compile_result compile(std::string_view code, scompiled_data& data_out, sbytecode& code_out);

	private:
		compile_result m_result = compile_result_ok;
	};

} //- slang