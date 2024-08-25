#pragma once
#include "slang_config.hpp"

namespace slang
{
	//------------------------------------------------------------------------------------------------------------------------
	struct slanguage final
	{
		static constexpr stringview_t C_INVALID_OR_UNDEFINED = "";
		static constexpr stringview_t C_LITERAL_EOF = "\0";
		static constexpr stringview_t C_LITERAL_EQUAL_EQUAL = "==";
		static constexpr stringview_t C_LITERAL_SMALLER_EQUAL = "<=";
		static constexpr stringview_t C_LITERAL_GREATER_EQUAL = ">=";
		static constexpr stringview_t C_LITERAL_NOT_EQUAL = "!=";
		static constexpr stringview_t C_LITERAL_EQUAL = "=";
		static constexpr stringview_t C_LITERAL_EXCLAMATION = "!";
		static constexpr stringview_t C_LITERAL_COLON = ":";
		static constexpr stringview_t C_LITERAL_SEMICOLON = ";";
		static constexpr stringview_t C_LITERAL_COMMA = ",";
		static constexpr stringview_t C_LITERAL_DOT = ".";
		static constexpr stringview_t C_LITERAL_LEFT_BRACKET = "[";
		static constexpr stringview_t C_LITERAL_RIGHT_BRACKET = "]";
		static constexpr stringview_t C_LITERAL_LEFT_BRACE = "{";
		static constexpr stringview_t C_LITERAL_RIGHT_BRACE = "}";
		static constexpr stringview_t C_LITERAL_LEFT_PAREN = "(";
		static constexpr stringview_t C_LITERAL_RIGHT_PAREN = ")";
		static constexpr stringview_t C_LITERAL_MINUS = "-";
		static constexpr stringview_t C_LITERAL_PLUS = "+";
		static constexpr stringview_t C_LITERAL_STAR = "*";
		static constexpr stringview_t C_LITERAL_SLASH = "/";
		static constexpr stringview_t C_LITERAL_SMALLER = "<";
		static constexpr stringview_t C_LITERAL_GREATER = ">";
		static constexpr stringview_t C_TOKEN_TRUE = "true";
		static constexpr stringview_t C_TOKEN_FALSE = "false";
		static constexpr stringview_t C_TOKEN_NULL = "null";
		static constexpr stringview_t C_TOKEN_CLASS = "class";
		static constexpr stringview_t C_TOKEN_DEF = "def";
		static constexpr stringview_t C_TOKEN_RETURN = "return";
		static constexpr stringview_t C_TOKEN_VAR = "var";
		static constexpr stringview_t C_TOKEN_THIS = "this";

		//- Note: must be in sync with token_type enum
		//------------------------------------------------------------------------------------------------------------------------
		static constexpr array_t<stringview_t, token_type_count> C_TOKENS =
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

		static const array_t<stringview_t, token_type_count>& tokens();
		static constexpr char token_character(token_type type) { return C_TOKENS[type][0]; }
	};

} //- slang