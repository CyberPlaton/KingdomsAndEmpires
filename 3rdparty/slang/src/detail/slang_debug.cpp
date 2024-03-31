#include "slang_debug.hpp"
#define FMT_HEADER_ONLY
#include "3rdparty/fmt/format.h"

namespace slang
{
	namespace
	{
	} //- unnamed

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

		//------------------------------------------------------------------------------------------------------------------------
		std::string print_object(sobject* object)
		{
			return {};
		}

		//- Using std string for ease of use with fmt
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
			}
			return {};
		}

		//------------------------------------------------------------------------------------------------------------------------
		void print_simple_instruction(const char* name)
		{
			slang_print(detail::log_level_debug, true, fmt::format("[{}]", name).c_str());
		}

		//- Print the instruction along with the value it is pointing to
		//------------------------------------------------------------------------------------------------------------------------
		void print_constant_instruction(const char* name, const detail::schunk& chunk, uint64_t index)
		{
			uint64_t i = chunk.m_code[index + 1];

			slang_print(detail::log_level_debug, true, fmt::format("[{}][{}]", name, print_value(chunk.m_constants[i])).c_str());
		}

	} //- detail

	//------------------------------------------------------------------------------------------------------------------------
	void slang_print(detail::log_level level, bool force, const char* fmt)
	{
		if (slang_logger().m_log && (slang_logger().m_level < level || force))
		{
			slang_logger().m_log(level, fmt);
		}
	}

	//- Not a logging or error reporting function, thus ignores log level. However the logging function must have been set
	//------------------------------------------------------------------------------------------------------------------------
	void print_chunk(const detail::schunk& chunk)
	{
		uint64_t index = 0;

		while (index < chunk.m_code.size())
		{
			print_instruction((detail::opcode)chunk.m_code[index], chunk, index);
		}
	}

	//- Depending on the instruction the index is increased by a certain amount
	//------------------------------------------------------------------------------------------------------------------------
	void print_instruction(detail::opcode instruction, const detail::schunk& chunk, uint64_t& index)
	{
		using namespace detail;

		switch (instruction)
		{
		default:
			break;
		case opcode_noop:
		{
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
			print_constant_instruction(opcode_name(opcode_constant), chunk, index);
			break;
		}
		case opcode_variable:
		{
			break;
		}
		}
	}

} //- slang