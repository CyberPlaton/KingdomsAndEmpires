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
			//- literals
			token_type_none = 0,
			token_type_equality,		//- ==
			token_type_smaller_equal,	//- <=
			token_type_greater_equal,	//- >=
			token_type_not_equality,	//- !=
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
			token_type_directive,		//- #
			token_type_at,				//- @
			token_type_quote,			//- "
			token_type_eof,				//- \0

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
			token_type_dvec2,
			token_type_dvec3,
			token_type_dvec4,
			token_type_vec2,
			token_type_vec3,
			token_type_vec4,
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
			token_type_layout,
			token_type_invariant,
			token_type_precision,
			token_type_highp,
			token_type_mediump,
			token_type_lowp,
			token_type_return,
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
			token_type_true,
			token_type_false,
			token_type_fn,			//- 'fn'
			token_type_struct,		//- 'struct'

			token_type_error,
			token_type_identifier,	//- i.e. variable, function, struct names etc.
			token_type_number,		//- i.e. 1.2, 0, 0.25
			token_type_string,		//- i.e. "Hello World"

			token_type_count,
		};

		//------------------------------------------------------------------------------------------------------------------------
		struct slanguage final
		{
			//- Literals
			static constexpr stringview_t C_INVALID_OR_UNDEFINED	= "";
			static constexpr stringview_t C_LITERAL_EQUAL_EQUAL		= "==";
			static constexpr stringview_t C_LITERAL_SMALLER_EQUAL	= "<=";
			static constexpr stringview_t C_LITERAL_GREATER_EQUAL	= ">=";
			static constexpr stringview_t C_LITERAL_NOT_EQUAL		= "!=";
			static constexpr stringview_t C_LITERAL_EQUAL			= "=";
			static constexpr stringview_t C_LITERAL_EXCLAMATION		= "!";
			static constexpr stringview_t C_LITERAL_COLON			= ":";
			static constexpr stringview_t C_LITERAL_SEMICOLON		= ";";
			static constexpr stringview_t C_LITERAL_COMMA			= ",";
			static constexpr stringview_t C_LITERAL_DOT				= ".";
			static constexpr stringview_t C_LITERAL_LEFT_BRACKET	= "[";
			static constexpr stringview_t C_LITERAL_RIGHT_BRACKET	= "]";
			static constexpr stringview_t C_LITERAL_LEFT_BRACE		= "{";
			static constexpr stringview_t C_LITERAL_RIGHT_BRACE		= "}";
			static constexpr stringview_t C_LITERAL_LEFT_PAREN		= "(";
			static constexpr stringview_t C_LITERAL_RIGHT_PAREN		= ")";
			static constexpr stringview_t C_LITERAL_MINUS			= "-";
			static constexpr stringview_t C_LITERAL_PLUS			= "+";
			static constexpr stringview_t C_LITERAL_STAR			= "*";
			static constexpr stringview_t C_LITERAL_SLASH			= "/";
			static constexpr stringview_t C_LITERAL_SMALLER			= "<";
			static constexpr stringview_t C_LITERAL_GREATER			= ">";
			static constexpr stringview_t C_LITERAL_DIRECTIVE		= "#";
			static constexpr stringview_t C_LITERAL_AT				= "@";
			static constexpr stringview_t C_LITERAL_QUOTE			= "\"";
			static constexpr stringview_t C_LITERAL_EOF				= "\0";

			//- Types
			static constexpr stringview_t C_TOKEN_VOID		= "void";
			static constexpr stringview_t C_TOKEN_BOOL		= "bool";
			static constexpr stringview_t C_TOKEN_INT		= "int";
			static constexpr stringview_t C_TOKEN_UINT		= "uint";
			static constexpr stringview_t C_TOKEN_FLOAT		= "float";
			static constexpr stringview_t C_TOKEN_DOUBLE	= "double";
			static constexpr stringview_t C_TOKEN_BVEC2		= "bvec2";
			static constexpr stringview_t C_TOKEN_BVEC3		= "bvec3";
			static constexpr stringview_t C_TOKEN_BVEC4		= "bvec4";
			static constexpr stringview_t C_TOKEN_IVEC2		= "ivec2";
			static constexpr stringview_t C_TOKEN_IVEC3		= "ivec3";
			static constexpr stringview_t C_TOKEN_IVEC4		= "ivec4";
			static constexpr stringview_t C_TOKEN_UVEC2		= "uvec2";
			static constexpr stringview_t C_TOKEN_UVEC3		= "uvec3";
			static constexpr stringview_t C_TOKEN_UVEC4		= "uvec4";
			static constexpr stringview_t C_TOKEN_DVEC2		= "dvec2";
			static constexpr stringview_t C_TOKEN_DVEC3		= "dvec3";
			static constexpr stringview_t C_TOKEN_DVEC4		= "dvec4";
			static constexpr stringview_t C_TOKEN_VEC2		= "vec2";
			static constexpr stringview_t C_TOKEN_VEC3		= "vec3";
			static constexpr stringview_t C_TOKEN_VEC4		= "vec4";
			static constexpr stringview_t C_TOKEN_MAT2		= "mat2";
			static constexpr stringview_t C_TOKEN_MAT3		= "mat3";
			static constexpr stringview_t C_TOKEN_MAT4		= "mat4";
			static constexpr stringview_t C_TOKEN_SAMPLER1D = "sampler1D";
			static constexpr stringview_t C_TOKEN_SAMPLER2D = "sampler2D";

			//- Keywords
			static constexpr stringview_t C_TOKEN_ATTRIBUTE	= "attribute";
			static constexpr stringview_t C_TOKEN_CONST		= "const";
			static constexpr stringview_t C_TOKEN_UNIFORM	= "uniform";
			static constexpr stringview_t C_TOKEN_VARYING	= "varying";
			static constexpr stringview_t C_TOKEN_BREAK		= "break";
			static constexpr stringview_t C_TOKEN_CONTINUE	= "continue";
			static constexpr stringview_t C_TOKEN_DISCARD	= "discard";
			static constexpr stringview_t C_TOKEN_FOR		= "for";
			static constexpr stringview_t C_TOKEN_WHILE		= "while";
			static constexpr stringview_t C_TOKEN_DO		= "do";
			static constexpr stringview_t C_TOKEN_IF		= "if";
			static constexpr stringview_t C_TOKEN_ELSE		= "else";
			static constexpr stringview_t C_TOKEN_SWITCH	= "switch";
			static constexpr stringview_t C_TOKEN_CASE		= "case";
			static constexpr stringview_t C_TOKEN_DEFAULT	= "default";
			static constexpr stringview_t C_TOKEN_IN		= "in";
			static constexpr stringview_t C_TOKEN_OUT		= "out";
			static constexpr stringview_t C_TOKEN_INOUT		= "inout";
			static constexpr stringview_t C_TOKEN_LAYOUT	= "layout";
			static constexpr stringview_t C_TOKEN_INVARIANT	= "invariant";
			static constexpr stringview_t C_TOKEN_PRECISION	= "precision";
			static constexpr stringview_t C_TOKEN_HIGHP		= "highp";
			static constexpr stringview_t C_TOKEN_MEDIUMP	= "mediump";
			static constexpr stringview_t C_TOKEN_LOWP		= "lowp";
			static constexpr stringview_t C_TOKEN_RETURN	= "return";

			//- Reserved keywords for future use
			static constexpr stringview_t C_TOKEN_COMMON	= "common";
			static constexpr stringview_t C_TOKEN_PARTITION = "partition";
			static constexpr stringview_t C_TOKEN_ACTIVE	= "active";
			static constexpr stringview_t C_TOKEN_ASM		= "asm";
			static constexpr stringview_t C_TOKEN_CLASS		= "class";
			static constexpr stringview_t C_TOKEN_UNION		= "union";
			static constexpr stringview_t C_TOKEN_ENUM		= "enum";
			static constexpr stringview_t C_TOKEN_TYPEDEF	= "typedef";
			static constexpr stringview_t C_TOKEN_TEMPLATE	= "template";
			static constexpr stringview_t C_TOKEN_THIS		= "this";
			static constexpr stringview_t C_TOKEN_PACKED	= "packed";
			static constexpr stringview_t C_TOKEN_GOTO		= "goto";
			static constexpr stringview_t C_TOKEN_INLINE	= "inline";
			static constexpr stringview_t C_TOKEN_NOINLINE	= "noinline";
			static constexpr stringview_t C_TOKEN_VOLATILE	= "volatile";
			static constexpr stringview_t C_TOKEN_PUBLIC	= "public";
			static constexpr stringview_t C_TOKEN_STATIC	= "static";
			static constexpr stringview_t C_TOKEN_EXTERN	= "extern";
			static constexpr stringview_t C_TOKEN_EXTERNAL	= "external";
			static constexpr stringview_t C_TOKEN_INTERFACE = "interface";
			static constexpr stringview_t C_TOKEN_LONG		= "long";
			static constexpr stringview_t C_TOKEN_SHORT		= "short";
			static constexpr stringview_t C_TOKEN_HALF		= "half";
			static constexpr stringview_t C_TOKEN_FIXED		= "fixed";
			static constexpr stringview_t C_TOKEN_UNSIGNED	= "unsigned";
			static constexpr stringview_t C_TOKEN_SUPERP	= "superp";
			static constexpr stringview_t C_TOKEN_INPUT		= "input";
			static constexpr stringview_t C_TOKEN_OUTPUT	= "output";
			static constexpr stringview_t C_TOKEN_HVEC2		= "hvec2";
			static constexpr stringview_t C_TOKEN_HVEC3		= "hvec3";
			static constexpr stringview_t C_TOKEN_HVEC4		= "hvec4";
			static constexpr stringview_t C_TOKEN_FVEC2		= "fvec2";
			static constexpr stringview_t C_TOKEN_FVEC3		= "fvec3";
			static constexpr stringview_t C_TOKEN_FVEC4		= "fvec4";
			static constexpr stringview_t C_TOKEN_FILTER	= "filter";

			static constexpr stringview_t C_TOKEN_TRUE		= "true";
			static constexpr stringview_t C_TOKEN_FALSE		= "false";
			static constexpr stringview_t C_TOKEN_FN		= "fn";
			static constexpr stringview_t C_TOKEN_STRUCT	= "struct";

			static constexpr stringview_t C_TOKEN_ERROR		= "error";
			static constexpr stringview_t C_TOKEN_STRING	= "string";
			static constexpr stringview_t C_TOKEN_IDENTIFIER = "identifier";
			static constexpr stringview_t C_TOKEN_NUMBER	= "number";

			static constexpr array_t<stringview_t, token_type_count> C_TOKENS =
			{
				C_INVALID_OR_UNDEFINED,
				C_LITERAL_EQUAL_EQUAL,
				C_LITERAL_SMALLER_EQUAL,
				C_LITERAL_GREATER_EQUAL,
				C_LITERAL_NOT_EQUAL,
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
				C_LITERAL_DIRECTIVE,
				C_LITERAL_AT,
				C_LITERAL_QUOTE,
				C_LITERAL_EOF,

				//- Types
				C_TOKEN_VOID,
				C_TOKEN_BOOL,
				C_TOKEN_INT,
				C_TOKEN_UINT,
				C_TOKEN_FLOAT,
				C_TOKEN_DOUBLE,
				C_TOKEN_BVEC2,
				C_TOKEN_BVEC3,
				C_TOKEN_BVEC4,
				C_TOKEN_IVEC2,
				C_TOKEN_IVEC3,
				C_TOKEN_IVEC4,
				C_TOKEN_UVEC2,
				C_TOKEN_UVEC3,
				C_TOKEN_UVEC4,
				C_TOKEN_DVEC2,
				C_TOKEN_DVEC3,
				C_TOKEN_DVEC4,
				C_TOKEN_VEC2,
				C_TOKEN_VEC3,
				C_TOKEN_VEC4,
				C_TOKEN_MAT2,
				C_TOKEN_MAT3,
				C_TOKEN_MAT4,
				C_TOKEN_SAMPLER1D,
				C_TOKEN_SAMPLER2D,

				//- Keywords
				C_TOKEN_ATTRIBUTE,
				C_TOKEN_CONST,
				C_TOKEN_UNIFORM,
				C_TOKEN_VARYING,
				C_TOKEN_BREAK,
				C_TOKEN_CONTINUE,
				C_TOKEN_DISCARD,
				C_TOKEN_FOR,
				C_TOKEN_WHILE,
				C_TOKEN_DO,
				C_TOKEN_IF,
				C_TOKEN_ELSE,
				C_TOKEN_SWITCH,
				C_TOKEN_CASE,
				C_TOKEN_DEFAULT,
				C_TOKEN_IN,
				C_TOKEN_OUT,
				C_TOKEN_INOUT,
				C_TOKEN_LAYOUT,
				C_TOKEN_INVARIANT,
				C_TOKEN_PRECISION,
				C_TOKEN_HIGHP,
				C_TOKEN_MEDIUMP,
				C_TOKEN_LOWP,
				C_TOKEN_RETURN,

				//- Keywords reserved for future use
				C_TOKEN_COMMON,
				C_TOKEN_PARTITION,
				C_TOKEN_ACTIVE,
				C_TOKEN_ASM,
				C_TOKEN_CLASS,
				C_TOKEN_UNION,
				C_TOKEN_ENUM,
				C_TOKEN_TYPEDEF,
				C_TOKEN_TEMPLATE,
				C_TOKEN_THIS,
				C_TOKEN_PACKED,
				C_TOKEN_GOTO,
				C_TOKEN_INLINE,
				C_TOKEN_NOINLINE,
				C_TOKEN_VOLATILE,
				C_TOKEN_PUBLIC,
				C_TOKEN_STATIC,
				C_TOKEN_EXTERN,
				C_TOKEN_EXTERNAL,
				C_TOKEN_INTERFACE,
				C_TOKEN_LONG,
				C_TOKEN_SHORT,
				C_TOKEN_HALF,
				C_TOKEN_FIXED,
				C_TOKEN_UNSIGNED,
				C_TOKEN_SUPERP,
				C_TOKEN_INPUT,
				C_TOKEN_OUTPUT,
				C_TOKEN_HVEC2,
				C_TOKEN_HVEC3,
				C_TOKEN_HVEC4,
				C_TOKEN_FVEC2,
				C_TOKEN_FVEC3,
				C_TOKEN_FVEC4,
				C_TOKEN_FILTER,
				C_TOKEN_TRUE,
				C_TOKEN_FALSE,
				C_TOKEN_FN,
				C_TOKEN_STRUCT,

				C_TOKEN_ERROR,
				C_TOKEN_IDENTIFIER,
				C_TOKEN_NUMBER,
				C_TOKEN_STRING
			};

			static const array_t<stringview_t, token_type_count>& tokens();
			static constexpr char token_character(token_type type) { return C_TOKENS[type][0]; }
			static string_t token_type_text(token_type type);
			static void validate_token_mapping();
		};

		//------------------------------------------------------------------------------------------------------------------------
		struct stoken final
		{
			string_t m_text;
			unsigned m_line = 0;
			token_type m_type = token_type_none;
		};

		using token_stream_t = vector_t<stoken>;

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
		//- <function_parameters> := ( <function_parameter> ("," <function_parameter>)* )*;
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
		//- <qualifiers> ::= ( "in" | "out" | "inout" | "uniform" | "attribute" | "invariant" )+;
		//-
		//- <declaration_qualified_variable> ::= <qualifiers> <type> <identifier> ( "=" <expression> )? ";";
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

		namespace lexing
		{
			//- Configuration of source scanning.
			//------------------------------------------------------------------------------------------------------------------------
			struct sconfig final
			{

			};
		}

		namespace detail
		{
			//- Responsible for providing context and commonly used structures to parts of scanning code. Moreover it allows
			//- for customization and extensions through configuration.
			//------------------------------------------------------------------------------------------------------------------------
			class clexing_context final :
				private core::cnon_copyable,
				private core::cnon_movable
			{
			public:
				struct scursor
				{
					string_t m_text;
					unsigned m_current = 0;
					unsigned m_line = 0;
				};

				explicit clexing_context(const char* source, lexing::sconfig cfg = {});
				~clexing_context() = default;

				inline scursor& cursor() { return m_cursor; }
				inline token_stream_t& stream() { return m_stream; }
				[[nodiscard]] token_stream_t&& take_stream() { return std::move(m_stream); }
				inline lexing::sconfig& cfg() { return m_cfg; }
				inline string_t& code() { return m_code; }
				inline bool& result() { return m_result; }

				stoken next_token();
				void process_token(stoken&& token);
				char peek(unsigned lookahead = 0) const;
				char advance();

			private:
				scursor m_cursor;
				token_stream_t m_stream;
				lexing::sconfig m_cfg;
				string_t m_code;
				bool m_result;
			};

		} //- detail

		//- TODO:
		//- 1. Process "fn" to token_type_function and "struct" to token_type_struct
		//- 2. Process data  types such as "vec2" or "float" into correct token_type´s
		//------------------------------------------------------------------------------------------------------------------------
		class ctiny_shader_language_lexer final
		{
		public:
			explicit ctiny_shader_language_lexer(const char* source, lexing::sconfig cfg = {});
			~ctiny_shader_language_lexer() = default;

			bool scan();

			[[nodiscard]] token_stream_t&& stream() { return std::move(m_ctx.take_stream()); }

		private:
			detail::clexing_context m_ctx;
		};

	} //- tsl

} //- sm