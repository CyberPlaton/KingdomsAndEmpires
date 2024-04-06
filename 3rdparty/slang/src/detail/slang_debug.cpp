#include "slang_debug.hpp"
#include "slang_object.hpp"
#define FMT_HEADER_ONLY
#include "3rdparty/fmt/format.h"

namespace slang
{
	namespace
	{
		constexpr stringview_t C_EMPTY = "";
		constexpr stringview_t C_TRUE = "True";
		constexpr stringview_t C_FALSE = "False";

		//------------------------------------------------------------------------------------------------------------------------
		std::string print_constant_instruction(const char* name, const detail::schunk& chunk, uint64_t index)
		{
			uint64_t i = chunk.m_code[index + 1];

			return fmt::format("[{}][{}]", name, debug::print_value(chunk.m_constants[i]));
		}

		//------------------------------------------------------------------------------------------------------------------------
		std::string print_simple_instruction(const char* name)
		{
			return fmt::format("[{}]", name);
		}

		//------------------------------------------------------------------------------------------------------------------------
		std::string print_function(const sfunction* object)
		{
			return fmt::format("[function][{}]", object->m_name.c_str());
		}

		//------------------------------------------------------------------------------------------------------------------------
		std::string print_struct(const sstruct* object)
		{
			return "<not implemented>";
		}

		//------------------------------------------------------------------------------------------------------------------------
		std::string print_string(const sstring* object)
		{
			return fmt::format("[string][{}]", object->m_string.c_str());
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	void slang_print(detail::log_level level, bool force, const char* fmt)
	{
		if (slang_logger().m_log && (slang_logger().m_level < level || force))
		{
			slang_logger().m_log(level, fmt);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void slang_log(detail::log_level level, const char* fmt)
	{
		slang_print(level, false, fmt);
	}

	namespace detail
	{
		//------------------------------------------------------------------------------------------------------------------------
		static const char* opcode_name(opcode code)
		{
			constexpr stringview_t C_NOOP		= "noop";
			constexpr stringview_t C_RETURN		= "return";
			constexpr stringview_t C_ADD		= "add";
			constexpr stringview_t C_SUB		= "subtract";
			constexpr stringview_t C_MUL		= "multiply";
			constexpr stringview_t C_DIV		= "divide";
			constexpr stringview_t C_CONST		= "constant";
			constexpr stringview_t C_VAR		= "variable";

			switch (code)
			{
			default:
				break;
			case detail::opcode_noop:		return C_NOOP.data();
			case detail::opcode_return:		return C_RETURN.data();
			case detail::opcode_add:		return C_ADD.data();
			case detail::opcode_subtract:	return C_SUB.data();
			case detail::opcode_multiply:	return C_MUL.data();
			case detail::opcode_divide:		return C_DIV.data();
			case detail::opcode_constant:	return C_CONST.data();
			case detail::opcode_variable:	return C_VAR.data();
			}
			return {};
		}

		//------------------------------------------------------------------------------------------------------------------------
		static const char* tokentype_name(token_type type)
		{
			constexpr stringview_t C_UNDEF		= "<undefined>";
			constexpr stringview_t C_NONE		= "none";
			constexpr stringview_t C_EOF		= "eof";
			constexpr stringview_t C_ERR		= "error";
			constexpr stringview_t C_ID			= "identifier";
			constexpr stringview_t C_NUM		= "number";
			constexpr stringview_t C_STR		= "string";
			constexpr stringview_t C_EQUAL		= "equal";
			constexpr stringview_t C_EXCL		= "exclamation";
			constexpr stringview_t C_COL		= "colon";
			constexpr stringview_t C_SEMICOL	= "semicolon";
			constexpr stringview_t C_COMMA		= "comma";
			constexpr stringview_t C_DOT		= "dot";
			constexpr stringview_t C_LBRACK		= "lbracket";
			constexpr stringview_t C_RBRACK		= "rbracket";
			constexpr stringview_t C_LBRACE		= "lbrace";
			constexpr stringview_t C_RBRACE		= "rbrace";
			constexpr stringview_t C_LPAR		= "lparen";
			constexpr stringview_t C_RPAR		= "rparen";
			constexpr stringview_t C_MIN		= "minus";
			constexpr stringview_t C_PLUS		= "plus";
			constexpr stringview_t C_STAR		= "start";
			constexpr stringview_t C_SLASH		= "slash";
			constexpr stringview_t C_SMALL		= "smaller";
			constexpr stringview_t C_GREAT		= "greater";
			constexpr stringview_t C_EQUALITY	= "equality";
			constexpr stringview_t C_SMALLEQUAL = "smallerequal";
			constexpr stringview_t C_GREATEQUAL = "greaterequal";
			constexpr stringview_t C_TRUE		= "true";
			constexpr stringview_t C_FALSE		= "false";
			constexpr stringview_t C_NULL		= "null";
			constexpr stringview_t C_CLASS		= "class";
			constexpr stringview_t C_DEF		= "def";
			constexpr stringview_t C_RET		= "return";
			constexpr stringview_t C_VAR		= "var";
			constexpr stringview_t C_THIS		= "this";

			switch (type)
			{
			case token_type_none:			return C_NONE.data();
			case token_type_eof:			return C_EOF.data();
			case token_type_error:			return C_ERR.data();
			case token_type_identifier:		return C_ID.data();
			case token_type_number:			return C_NUM.data();
			case token_type_string:			return C_STR.data();
			case token_type_equal:			return C_EQUAL.data();
			case token_type_exclamation:	return C_EXCL.data();
			case token_type_colon:			return C_COL.data();
			case token_type_semicolon:		return C_SEMICOL.data();
			case token_type_comma:			return C_COMMA.data();
			case token_type_dot:			return C_DOT.data();
			case token_type_left_bracket:	return C_LBRACK.data();
			case token_type_right_bracket:	return C_RBRACK.data();
			case token_type_left_brace:		return C_LBRACE.data();
			case token_type_right_brace:	return C_RBRACE.data();
			case token_type_left_paren:		return C_LPAR.data();
			case token_type_right_paren:	return C_RPAR.data();
			case token_type_minus:			return C_MIN.data();
			case token_type_plus:			return C_PLUS.data();
			case token_type_star:			return C_STAR.data();
			case token_type_slash:			return C_SLASH.data();
			case token_type_smaller:		return C_SMALL.data();
			case token_type_greater:		return C_GREAT.data();
			case token_type_equality:		return C_EQUALITY.data();
			case token_type_smaller_equal:	return C_SMALLEQUAL.data();
			case token_type_greater_equal:	return C_GREATEQUAL.data();
			case token_type_true:			return C_TRUE.data();
			case token_type_false:			return C_FALSE.data();
			case token_type_null:			return C_NULL.data();
			case token_type_class:			return C_CLASS.data();
			case token_type_def:			return C_DEF.data();
			case token_type_return:			return C_RET.data();
			case token_type_var:			return C_VAR.data();
			case token_type_this:			return C_THIS.data();
			default:
				break;
			}
			return C_UNDEF.data();
		}

	} //- detail

	namespace debug
	{
		//------------------------------------------------------------------------------------------------------------------------
		std::string print_token(const detail::stoken& token)
		{
			constexpr stringview_t C_NL = "\n";

			return fmt::format("[{}:\"{}\"]", tokentype_name(token.m_type), token.m_text.c_str());
		}

		//------------------------------------------------------------------------------------------------------------------------
		std::string print_token_stream(const detail::stoken_stream& stream)
		{
			std::string out = "Tokenstream\n";
			uint32_t line = 0;

			//- print line 0
			out = fmt::format("{}{}\t", out, line);

			for (const auto& token : stream.m_stream)
			{
				//- increment to next line if required
				if (line < token.m_line)
				{
					line = token.m_line;
					out = fmt::format("{}\n{}\t", out, line);
				}
				out = fmt::format("{}{}", out, print_token(token));
			}

			return out;
		}

		//------------------------------------------------------------------------------------------------------------------------
		std::string print_chunk(const detail::schunk& chunk)
		{
			std::string out;
			uint64_t index = 0;

			while (index < chunk.m_code.size())
			{
				out = fmt::format("{}{}", out, print_instruction((detail::opcode)chunk.m_code[index], chunk, index));
			}

			return out;
		}

		//------------------------------------------------------------------------------------------------------------------------
		std::string print_value(const svalue& value)
		{
			switch (value.m_type)
			{
			case value_type_boolean:
			{
				return fmt::format("{}", value.get<bool>() ? C_TRUE.data() : C_FALSE.data());
			}
			case value_type_integer:
			{
				return fmt::format("{}", value.get<int>());
			}
			case value_type_float:
			{
				return fmt::format("{}", value.get<float>());
			}
			case value_type_object:
			{
				return print_object(value.get<sobject*>());
			}
			default:
				break;
			}
			return C_EMPTY.data();
		}

		//------------------------------------------------------------------------------------------------------------------------
		std::string print_object(const sobject* object)
		{
			switch (object->m_type)
			{
			case object_type_function:
			{
				return print_function(object->as<sfunction>());
			}
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		std::string print_instruction(detail::opcode instruction, const detail::schunk& chunk, uint64_t& index)
		{
			using namespace detail;

			std::string out;

			switch (instruction)
			{
			default:
			case opcode_noop:
			{
				++index;
				break;
			}
			case opcode_return:
			{
				break;
			}
			case opcode_add:
			{
				break;
			}
			case opcode_subtract:
			{
				break;
			}
			case opcode_multiply:
			{
				break;
			}
			case opcode_divide:
			{
				break;
			}
			case opcode_constant:
			{
				out = print_constant_instruction(opcode_name(opcode_constant), chunk, index);

				//- increase one for bytecode and value
				index += 2;
				break;
			}
			case opcode_variable:
			{
				break;
			}
			}
			return out;
		}

	} //- debug

} //- slang