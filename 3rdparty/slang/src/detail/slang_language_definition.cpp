#include "slang_language_definition.hpp"

namespace slang
{
	//------------------------------------------------------------------------------------------------------------------------
	const array_t<stringview_t, token_type_count>& slanguage::tokens()
	{
		return C_TOKENS;
	}

} //- slang