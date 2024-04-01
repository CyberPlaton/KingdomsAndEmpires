#pragma once
#include "slang_config.hpp"

namespace slang
{
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
		sfunction(stringview_t name) : sobject(object_type_function), m_name(name.data()){}

		detail::schunk m_chunk;
		string_t m_name;
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sstruct final : sobject
	{
		sstruct() : sobject(object_type_struct) {}
	};

} //- slang