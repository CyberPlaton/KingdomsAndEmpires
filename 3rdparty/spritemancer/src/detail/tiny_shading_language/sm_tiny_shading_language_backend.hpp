#pragma once
#include <core.h>
#include "../sm_config.hpp"

namespace sm
{
	class itiny_shader_compiler;
	using tiny_shader_compiler_ref_t = ref_t<itiny_shader_compiler>;

	//------------------------------------------------------------------------------------------------------------------------
	enum compile_result : uint8_t
	{
		compile_result_ok = 0,
		compile_result_fail = 255,
	};

	//- Interface class responsible for compiling tiny shader language to target shading language.
	//- BNF:
	//-<program> ::= { <directive> | <function> }
	//- <directive> :: = "#include" < string >
	//- <string> :: = '"' < filename > '"'
	//- < filename > :: = <char>{ <char> }
	//- <char> :: = any valid file name character
	//- 
	//- <annotated_function> ::= <annotations> <function>
	//- <annotations> :: = ( "@" < identifier > )*;
	//- 
	//- <function> :: = "fn" < identifier > "("[<parameters>] ")" "->" < type > "{" < body > "}"
	//- 
	//- < parameters > :: = <parameter> ( "," < parameter > )*
	//- <parameter> :: = < identifier> ":" < type >
	//- 
	//- <body> :: = { <statement> }
	//- <statement> :: = <variable_declaration> | <return_statement> | < expression> ";"
	//- 
	//- < variable_declaration > :: = "let" < identifier > "=" < expression > ";"
	//- 
	//- < return_statement > :: = "return" < expression > ";"
	//- 
	//- < expression > :: = <term>{ <operator> <term> }
	//- <term> :: = <identifier> | <literal> | <function_call>
	//- <function_call> :: = < identifier> "("[<arguments>] ")"
	//- < arguments > :: = <expression>{ "," < expression > }
	//- 
	//- <type> :: = "vec2" | "vec3" | "vec4" | "mat4" | "float" | "int" | "void"
	//- 
	//- < operator> :: = "+" | "-" | "*" | "/" | "%"
	//- < identifier > :: = <letter>{ <letter_or_digit> }
	//- <letter> :: = "a" | "b" | ... | "z" | "A" | "B" | ... | "Z" | "_"
	//- < letter_or_digit > :: = <letter> | "0" | "1" | ... | "9"
	//- 
	//- < literal > :: = <numeric_literal>
	//- <numeric_literal> :: = <integer_literal> | <floating_point_literal>
	//- <integer_literal> :: = <digit>{ <digit> }
	//- <floating_point_literal> :: = < integer_literal> "." < integer_literal >
	//- <digit> :: = "0" | "1" | ... | "9"
	//------------------------------------------------------------------------------------------------------------------------
	class itiny_shader_compiler
	{
	public:
		virtual compile_result	compile(stringview_t) = 0;
		virtual compile_result	compile(const memory_ref_t&) = 0;
		virtual string_t		emit() = 0;
	};

} //- sm