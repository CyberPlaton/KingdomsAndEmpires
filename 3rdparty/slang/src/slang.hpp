#pragma once
#include "detail/slang_debug.hpp"
#include "detail/slang_compiler.hpp"
#include "detail/slang_vm.hpp"

namespace slang
{
	cslang_state* slang_open();
	compile_result slang_compile(cslang_state* state, stringview_t code);
	void slang_close(cslang_state* state);

	//- Holds allocated values that are allocated dynamically and can change their values
	//------------------------------------------------------------------------------------------------------------------------
	struct svariables
	{
		umap_t<uint64_t, svalue> m_values;
	};

} //- slang