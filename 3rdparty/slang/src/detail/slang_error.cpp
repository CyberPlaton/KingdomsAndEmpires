#include "slang_error.hpp"

namespace slang
{
	namespace
	{
		//- Note: must be in sync with error_type enum
		//------------------------------------------------------------------------------------------------------------------------
		constexpr array_t<stringview_t, error_type_count> C_ERRORS =
		{
			C_INVALID_OR_UNDEFINED,
			C_ERROR_UNRECOGNIZED_LITERAL,
			C_ERROR_UNTERMINATED_STRING,
			C_ERROR_MULTILINE_STRING,
			C_ERROR_EOF_EXPECTED
		};

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	const array_t<stringview_t, error_type_count>& serrors::errors()
	{
		return C_ERRORS;
	}

} //- slang