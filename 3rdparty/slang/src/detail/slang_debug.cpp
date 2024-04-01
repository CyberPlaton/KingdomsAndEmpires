#include "slang_debug.hpp"
#include "slang_object.hpp"
#define FMT_HEADER_ONLY
#include "3rdparty/fmt/format.h"

namespace slang
{
	namespace
	{
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
		const char* opcode_name(opcode code)
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

	} //- detail

	namespace debug
	{
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
				return fmt::format("{}", value.get<bool>() ? "True" : "False");
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
			return {};
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