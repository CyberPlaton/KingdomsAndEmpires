#include "slang_language_definition.hpp"

namespace slang
{
	namespace
	{
		constexpr stringview_t C_INVALID_OR_UNDEFINED	= "\0";
		constexpr stringview_t C_LITERAL_EOF			= "\0";
		constexpr stringview_t C_LITERAL_EQUAL_EQUAL	= "==";
		constexpr stringview_t C_LITERAL_SMALLER_EQUAL	= "<=";
		constexpr stringview_t C_LITERAL_GREATER_EQUAL	= ">=";
		constexpr stringview_t C_LITERAL_NOT_EQUAL		= "!=";
		constexpr stringview_t C_LITERAL_EQUAL			= "=";
		constexpr stringview_t C_LITERAL_EXCLAMATION	= "!";
		constexpr stringview_t C_LITERAL_COLON			= ":";
		constexpr stringview_t C_LITERAL_SEMICOLON		= ";";
		constexpr stringview_t C_LITERAL_COMMA			= ",";
		constexpr stringview_t C_LITERAL_DOT			= ".";
		constexpr stringview_t C_LITERAL_LEFT_BRACKET	= "[";
		constexpr stringview_t C_LITERAL_RIGHT_BRACKET	= "]";
		constexpr stringview_t C_LITERAL_LEFT_BRACE		= "{";
		constexpr stringview_t C_LITERAL_RIGHT_BRACE	= "}";
		constexpr stringview_t C_LITERAL_LEFT_PAREN		= "(";
		constexpr stringview_t C_LITERAL_RIGHT_PAREN	= ")";
		constexpr stringview_t C_LITERAL_MINUS			= "-";
		constexpr stringview_t C_LITERAL_PLUS			= "+";
		constexpr stringview_t C_LITERAL_STAR			= "*";
		constexpr stringview_t C_LITERAL_SLASH			= "/";
		constexpr stringview_t C_LITERAL_SMALLER		= "<";
		constexpr stringview_t C_LITERAL_GREATER		= ">";
		constexpr stringview_t C_TOKEN_TRUE				= "true";
		constexpr stringview_t C_TOKEN_FALSE			= "false";
		constexpr stringview_t C_TOKEN_NULL				= "null";
		constexpr stringview_t C_TOKEN_CLASS			= "class";
		constexpr stringview_t C_TOKEN_DEF				= "def";
		constexpr stringview_t C_TOKEN_RETURN			= "return";
		constexpr stringview_t C_TOKEN_VAR				= "var";
		constexpr stringview_t C_TOKEN_THIS				= "this";

		//- Note: must be in sync with token_type enum
		//------------------------------------------------------------------------------------------------------------------------
		constexpr array_t<stringview_t, token_type_count> C_TOKENS =
		{
			C_INVALID_OR_UNDEFINED, //- none
			C_LITERAL_EOF,
			C_INVALID_OR_UNDEFINED, //- error
			C_INVALID_OR_UNDEFINED, //- identifier
			C_INVALID_OR_UNDEFINED, //- number
			C_INVALID_OR_UNDEFINED, //- string
			C_LITERAL_EQUAL,
			C_LITERAL_EXCLAMATION,
			C_LITERAL_COLON,
			C_LITERAL_SEMICOLON,
			C_LITERAL_COMMA,
			C_LITERAL_DOT,
			C_LITERAL_LEFT_BRACKET,
			C_LITERAL_RIGHT_BRACKET,
			C_LITERAL_LEFT_BRACE,
			C_LITERAL_RIGHT_BRACE,
			C_LITERAL_LEFT_PAREN,
			C_LITERAL_RIGHT_PAREN,
			C_LITERAL_MINUS,
			C_LITERAL_PLUS,
			C_LITERAL_STAR,
			C_LITERAL_SLASH,
			C_LITERAL_SMALLER,
			C_LITERAL_GREATER,
			C_LITERAL_EQUAL_EQUAL,
			C_LITERAL_NOT_EQUAL,
			C_LITERAL_SMALLER_EQUAL,
			C_LITERAL_GREATER_EQUAL,

			C_TOKEN_TRUE,
			C_TOKEN_FALSE,
			C_TOKEN_NULL,
			C_TOKEN_CLASS,
			C_TOKEN_DEF,
			C_TOKEN_VAR,
			C_TOKEN_THIS,
			C_TOKEN_RETURN
		};

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	const array_t<stringview_t, token_type_count>& slanguage::tokens()
	{
		return C_TOKENS;
	}

} //- slang