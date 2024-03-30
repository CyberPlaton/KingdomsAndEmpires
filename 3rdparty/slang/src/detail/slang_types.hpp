#pragma once
#include "slang_config.hpp"

namespace slang
{
	//------------------------------------------------------------------------------------------------------------------------
	enum value_type : uint8_t
	{
		value_type_null = 0,
		value_type_integer,
		value_type_float,
		value_type_boolean,
		value_type_string,
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TValue>
	struct ivalue
	{
		explicit ivalue(const TValue& value, value_type type) :
			m_value(value), m_type(type)
		{
 		}

		explicit ivalue(TValue&& value, value_type type) :
			m_value(std::move(value)), m_type(type)
		{
		}

		TValue m_value;
		const value_type m_type;
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct snull final : ivalue<int>
	{
		snull() : ivalue<int>(0, value_type_null) {}
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sinteger final : ivalue<int>
	{
		sinteger() : ivalue<int>(0, value_type_integer){}
		sinteger(int value) : ivalue<int>(value, value_type_integer){}
		sinteger(const sinteger& value) : ivalue<int>(value.m_value, value_type_integer){}
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sfloat final : ivalue<float>
	{
		sfloat() : ivalue<float>(0.0f, value_type_float) {}
		sfloat(float value) : ivalue<float>(value, value_type_float) {}
		sfloat(const sfloat& value) : ivalue<float>(value.m_value, value_type_float) {}
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sboolean final : ivalue<bool>
	{
		sboolean() : ivalue<bool>(0, value_type_boolean) {}
		sboolean(bool value) : ivalue<bool>(value, value_type_boolean) {}
		sboolean(const sboolean& value) : ivalue<bool>(value.m_value, value_type_boolean) {}
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sstring final : ivalue<string_t>
	{
		sstring() : ivalue<string_t>(0, value_type_string) {}
		sstring(string_t value) : ivalue<string_t>(value, value_type_string) {}
		sstring(const sstring& value) : ivalue<string_t>(value.m_value, value_type_string) {}
	};

} //- slang