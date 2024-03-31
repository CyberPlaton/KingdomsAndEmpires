#pragma once
#include "slang_config.hpp"

namespace slang
{
	struct sobject;

	//------------------------------------------------------------------------------------------------------------------------
	enum value_type : uint8_t
	{
		value_type_null = 0,
		value_type_integer,
		value_type_float,
		value_type_boolean,
		value_type_object,
	};

	//- Struct holding one of possible primitive value types. 
	//------------------------------------------------------------------------------------------------------------------------
	struct svalue
	{
		variant_t<int, float, bool, sobject*> as;
		value_type m_type;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TValue>
	inline static svalue make_value(TValue value, value_type type)
	{
		return { value, type };
	}

} //- slang