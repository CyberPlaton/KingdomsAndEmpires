#include "slang_value.hpp"

namespace slang
{
	//------------------------------------------------------------------------------------------------------------------------
	cvalue::cvalue() : m_type(object_type_none), m_marked(false)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	cvalue::cvalue(object_type type) : m_type(type), m_marked(false)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	cvalue::~cvalue()
	{

	}

	cvalue create_string_object()
	{
		return cvalue(object_type_string);
	}

	cvalue create_null_object()
	{
		return cvalue(object_type_null);
	}

	cvalue create_struct_object()
	{
		return cvalue(object_type_struct);
	}

	cvalue create_function_object()
	{
		return cvalue(object_type_function);
	}

} //- slang