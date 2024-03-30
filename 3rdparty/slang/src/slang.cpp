#include "slang.hpp"
#include "detail/slang_types.hpp"

namespace slang
{
	namespace
	{
		inline static uint64_t s_variable_identifier = 0;

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TValue>
		uint64_t emit_constant(TValue value, value_type type, scompiled_data& data)
		{
			uint64_t i = 0;
			switch (type)
			{
			case value_type_integer:
			{
				i = data.m_constants.m_integers.size();
				data.m_constants.m_integers.emplace_back(value);
				break;
			}
			case value_type_float:
			{
				i = data.m_constants.m_floats.size();
				data.m_constants.m_floats.emplace_back(value);
				break;
			}
			case value_type_boolean:
			{
				i = data.m_constants.m_booleans.size();
				data.m_constants.m_booleans.emplace_back(value);
				break;
			}
			case value_type_string:
			{
				i = data.m_constants.m_strings.size();
				data.m_constants.m_strings.emplace_back(value);
				break;
			}
			default:
			case value_type_null: return uint64_t(-1);
			}
			return i;
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TValue>
		uint64_t emit_variable(TValue value, value_type type, scompiled_data& data)
		{
			switch (type)
			{
			case value_type_integer:
			{
				data.m_variables.m_integers.emplace_back(++s_variable_identifier, value);
				break;
			}
			case value_type_float:
			{
				data.m_variables.m_floats.emplace_back(++s_variable_identifier, value);
				break;
			}
			case value_type_boolean:
			{
				data.m_variables.m_booleans.emplace_back(++s_variable_identifier, value);
				break;
			}
			case value_type_string:
			{
				data.m_variables.m_strings.emplace_back(++s_variable_identifier, value);
				break;
			}
			case value_type_null:
			{
				data.m_variables.m_nulls[++s_variable_identifier];
				break;
			}
			default: return uint64_t(-1);
			}
			return s_variable_identifier;
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	compile_result ccompiler::compile(std::string_view code, scompiled_data& data_out, sbytecode& code_out)
	{
		for (auto i = 0; i < code.length(); ++i)
		{
			const auto& c = code[i];

			//-- scanning
		}

		return m_result;
	}

} //- slang