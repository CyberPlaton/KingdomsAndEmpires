#pragma once
#include <core.h>
#include "../sm_config.hpp"

namespace sm
{
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


	};

} //- sm