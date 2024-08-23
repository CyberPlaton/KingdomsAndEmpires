#pragma once
#include "slang_core.hpp"

namespace slang
{
	//- Base object of the language. Does hold a primitive value such as a POD, string, function or structure object and
	//- can be marked as unreachable for garbage collector and deallocated.
	//------------------------------------------------------------------------------------------------------------------------
	class cvalue final
	{
	public:
		cvalue();
		cvalue(object_type type);
		~cvalue();

	private:
		cany m_data;
		object_type m_type;
		bool m_marked;
	};

} //- slang
