#pragma once
#include "slang_config.hpp"

namespace slang
{
	//- Error string are lfet here just in case. If there arent needed move them to cpp file.
	//------------------------------------------------------------------------------------------------------------------------
	struct serrors final
	{
		static constexpr stringview_t C_INVALID_OR_UNDEFINED = "";
		static constexpr stringview_t C_ERROR_UNRECOGNIZED_LITERAL	= "An unrecognized literal was encountered";
		static constexpr stringview_t C_ERROR_UNTERMINATED_STRING	= "Unterminated string. String declarations must end with '\"'";
		static constexpr stringview_t C_ERROR_MULTILINE_STRING		= "Multiline strings are not supported. Make sure the string ends with '\"' on the same line it was declared on";
		static constexpr stringview_t C_ERROR_EOF_EXPECTED			= "Expected 'end of file' expression";

		static const array_t<stringview_t, error_type_count>& errors();
	};

} //- slang