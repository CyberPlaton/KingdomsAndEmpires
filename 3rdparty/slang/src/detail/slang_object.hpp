#pragma once
#include "slang_config.hpp"

namespace slang
{
	//------------------------------------------------------------------------------------------------------------------------
	enum object_type : uint8_t
	{
		object_type_none = 0,
		object_type_null,
		object_type_string,
		object_type_function,
		object_type_struct
	};

	//- Base of a language construct
	//------------------------------------------------------------------------------------------------------------------------
	struct sobject
	{
		sobject() : m_marked(false), m_type(object_type_none) {}
		sobject(object_type type) : m_marked(false), m_type(type) {}

		bool m_marked;
		object_type m_type;
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct snull final : public sobject
	{
		snull() : sobject(object_type_null) {}
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sstring final : sobject
	{
		sstring() : sobject(object_type_string) {}

		string_t m_string;
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sfunction final : sobject
	{
		sfunction() : sobject(object_type_function) {}
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sstruct final : sobject
	{
		sstruct() : sobject(object_type_struct) {}
	};

	//- Upcast basic object
	//------------------------------------------------------------------------------------------------------------------------
	template<typename TObject>
	inline static TObject* object_as(sobject* object)
	{
		return reinterpret_cast<TObject*>(object);
	}

} //- slang