#pragma once
#include <core.h>
#include "../sm_config.hpp"

namespace sm
{
	namespace tsl
	{
		//------------------------------------------------------------------------------------------------------------------------
		enum token_type : uint8_t
		{
			token_type_none = 0,
			token_type_eof,
			token_type_error,

			token_type_identifier,		//- i.e. variable, function, struct names etc.
			token_type_number,			//- i.e. 1.2, 0, 0.25
			token_type_string,			//- i.e. "Hello World"
			token_type_equal,			//- =
			token_type_exclamation,		//- !
			token_type_colon,			//- :
			token_type_semicolon,		//- ;
			token_type_comma,			//- ,
			token_type_dot,				//- .
			token_type_left_bracket,	//- [
			token_type_right_bracket,	//- ]
			token_type_left_brace,		//- {
			token_type_right_brace,		//- }
			token_type_left_paren,		//- (
			token_type_right_paren,		//- )

			token_type_minus,			//- -
			token_type_plus,			//- +
			token_type_star,			//- *
			token_type_slash,			//- /

			token_type_smaller,			//- <
			token_type_greater,			//- >
			token_type_equality,		//- ==
			token_type_not_equality,	//- !=
			token_type_smaller_equal,	//- <=
			token_type_greater_equal,	//- >=

			//- types
			token_type_void,
			token_type_bool,
			token_type_int,
			token_type_uint,
			token_type_float,
			token_type_double,

			token_type_bvec2,
			token_type_bvec3,
			token_type_bvec4,
			token_type_ivec2,
			token_type_ivec3,
			token_type_ivec4,
			token_type_uvec2,
			token_type_uvec3,
			token_type_uvec4,
			token_type_vec2,
			token_type_vec3,
			token_type_vec4,
			token_type_dvec2,
			token_type_dvec3,
			token_type_dvec4,

			token_type_mat2,
			token_type_mat3,
			token_type_mat4,

			token_type_sampler1D,
			token_type_sampler2D,

			//- keywords
			token_type_attribute,
			token_type_const,
			token_type_uniform,
			token_type_varying,
			token_type_break,
			token_type_continue,
			token_type_discard,
			token_type_for,
			token_type_while,
			token_type_do,
			token_type_if,
			token_type_else,
			token_type_switch,
			token_type_case,
			token_type_default,
			token_type_in,
			token_type_out,
			token_type_inout,
			token_type_struct,
			token_type_layout,
			token_type_true,
			token_type_false,
			token_type_invariant,
			token_type_precision,
			token_type_highp,
			token_type_mediump,
			token_type_lowp,
			token_type_return,

			//- keywords, reserved for future use
			token_type_common,
			token_type_partition,
			token_type_active,
			token_type_asm,
			token_type_class,
			token_type_union,
			token_type_enum,
			token_type_typedef,
			token_type_template,
			token_type_this,
			token_type_packed,
			token_type_goto,
			token_type_inline,
			token_type_noinline,
			token_type_volatile,
			token_type_public,
			token_type_static,
			token_type_extern,
			token_type_external,
			token_type_interface,
			token_type_long,
			token_type_short,
			token_type_half,
			token_type_fixed,
			token_type_unsigned,
			token_type_superp,
			token_type_input,
			token_type_output,
			token_type_hvec2,
			token_type_hvec3,
			token_type_hvec4,
			token_type_fvec2,
			token_type_fvec3,
			token_type_fvec4,
			token_type_filter,

			token_type_count,
		};

	} //- tsl

	//- Responsible for parsing the TSL source file and constructing an abstract syntax tree usable by a compiler.
	//- More or less accurate BNF:
	//- 
	//- <program> := <directive>
	//- | <declaration_function>
	//- | <declaration_structure>
	//- | <declaration_variable>
	//- ;
	//- 
	//- <directive> := "#include" <literal_string>
	//- 			| "#pragma" <identifier>
	//- 			;
	//- 
	//- <declaration_function> := "fn" <identifier> "(" <function_parameters> ")" "->" <type>
	//- 					("@" <annotations_body> "@")?
	//- 					"{" <function_body> "}" ";";
	//- 
	//- <function_parameter> := <identifier> ":" <type>;
	//- 
	//- <function_parameters> := (<function_parameter>)*;
	//- 
	//- <declaration_annotation_member> := <type> <identifier> "=" <identifier> ";";
	//- 
	//- <annotations_body> := ( <declaration_annotation_member> )*;
	//- 
	//- <function_body> := (<statement>)*;
	//- 
	//- <declaration_structure_member> := <type> <identifier> ( "=" <expression> )? ";";
	//- 
	//- <structure_body> := ( <declaration_structure_member> )*;
	//- 
	//- <declaration_structure> := "struct" <identifier> "{" <structure_body> "}" ";";
	//- 
	//- <statement> := <declaration_variable>
	//- 			| <assignment>
	//- 			| <expression> ";"
	//- 			| <statement_return>
	//- 			;
	//- 
	//- <expression> := <identifier>
	//- 			| <literal>
	//- 			| <function_call>
	//- 			| <expression_binary>
	//- 			| <expression_unary>
	//- 			;
	//- 
	//- <declaration_variable> := <type> <identifier> "=" <expression> ";";
	//- 
	//- <assignment> := <identifier> "=" <expression> ";";
	//- 
	//- <statement_return> := "return" <expression> ";";
	//- 
	//- <function_arguments> := <expression> ("," <expression>)*;
	//- 
	//- <function_call> := <identifier> "(" ()* ")";
	//- 
	//- <operator_binary> := "+"
	//- 				| "-"
	//- 				| "*"
	//- 				| "/"
	//- 				| "=="
	//- 				| "!="
	//- 				| "<"
	//- 				| ">"
	//- 				| "<="
	//- 				| ">="
	//- 				;
	//- 
	//- <expression_binary> := <expression> <operator_binary> <expression>;
	//- 
	//-  <operator_unary> := "-" | "!";
	//- 
	//- <expression_unary> := <operator_unary> <expression>;
	//- 
	//- (Normally supported variable types in shading languages, roughly a superset of all supported backends)
	//- <type> := "int"
	//- 		| "float"
	//- 		| "vec2"
	//- 		| "vec3"
	//- 		| "vec4"
	//- 		| "mat2"
	//- 		| "mat3"
	//- 		| "mat4"
	//- 		| "sampler2D"
	//- 		;
	//- 
	//- <literal> := <literal_numeric>
	//- 		| <literal_boolean>
	//- 		| <literal_string>
	//- 		;
	//- 
	//- <literal_numeric>
	//- 
	//- <literal_boolean>
	//- 
	//- <literal_string> := (<character>)+;
	//-
	//- <character> := "a" ... "z"
	//-				| "A" ... "Z"
	//-				| "_"
	//-				;
	//------------------------------------------------------------------------------------------------------------------------
	class ctiny_shader_language_lexer final
	{
	public:
		using byte_code_t = vector_t<byte_t>;

	};

} //- sm