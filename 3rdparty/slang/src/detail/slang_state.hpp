#pragma once
#include "slang_config.hpp"
#include "slang_compiler.hpp"

namespace slang
{
	//------------------------------------------------------------------------------------------------------------------------
	class cstate final
	{
	public:
		cstate();
		~cstate();

		compile_result compile(stringview_t code);

	private:
	};

} //- slang