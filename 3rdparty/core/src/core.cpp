#include "core.hpp"

namespace core
{
	cuuid::cuuid()
	{

	}

	cuuid::cuuid(const string_t& uuid)
	{

	}

	cuuid::cuuid(::size_t seed)
	{

	}

	cuuid::cuuid(const cuuid& other)
	{
		copy_from(other);
		m_string = generate_string();
	}

	stringview_t cuuid::view() const
	{

	}


	void cuuid::generate(::size_t seed)
	{

	}

	void cuuid::parse_string(const string_t& uuid, array_t<unsigned char, 16u>& out)
	{

	}

	void cuuid::write_string(const array_t<unsigned char, 16>& data, string_t& out) const
	{

	}

	unsigned cuuid::hex2dec(char c)
	{

	}

	void cuuid::copy_to(cuuid& other)
	{

	}

	void cuuid::copy_from(const cuuid& other)
	{

	}

	int cuuid::compare(const cuuid& other) const
	{

	}

	string_t cuuid::generate_string() const
	{

	}

} //- core