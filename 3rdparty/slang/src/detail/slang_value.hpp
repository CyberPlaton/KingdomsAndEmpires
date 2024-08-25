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
		template<typename T>
		cvalue(object_type type, T value);
		~cvalue();

		inline cany& data() { return m_data; }
		inline object_type type() const { return m_type; }
		inline bool marked() const { return m_marked; }

	private:
		cany m_data;
		object_type m_type;
		bool m_marked;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	cvalue::cvalue(object_type type, T value) :
		m_type(type), m_data(value), m_marked(false)
	{
	}

} //- slang
