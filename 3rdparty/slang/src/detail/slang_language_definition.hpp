#pragma once
#include "slang_config.hpp"

namespace slang
{
	//------------------------------------------------------------------------------------------------------------------------
	struct slanguage final
	{
		static const array_t<stringview_t, token_type_count>& tokens();
	};

} //- slang