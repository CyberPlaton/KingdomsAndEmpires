#include "core_stringbuffer.hpp"

namespace core
{
	//------------------------------------------------------------------------------------------------------------------------
	cstring_buffer::~cstring_buffer()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cstring_buffer& cstring_buffer::write(const char* text)
	{
		return write("{}", text);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cstring_buffer& cstring_buffer::write(stringview_t text)
	{
		return write("{}", text.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	cstring_buffer& cstring_buffer::write(const string_t& text)
	{
		return write("{}", text.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cstring_buffer::reserve(uint64_t n)
	{
		m_buffer.reserve(n);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cstring_buffer::resize(uint64_t n)
	{
		m_buffer.resize(n);
	}

	//------------------------------------------------------------------------------------------------------------------------
	uint64_t cstring_buffer::size() const
	{
		return m_buffer.size();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cstring_buffer::empty() const
	{
		return size() == 0;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cstring_buffer::clear()
	{
		m_buffer.clear();
	}

	//------------------------------------------------------------------------------------------------------------------------
	const char* cstring_buffer::c_str()
	{
		write("{}", '\0'); return data();
	}

	//------------------------------------------------------------------------------------------------------------------------
	string_t cstring_buffer::string()
	{
		return string_t(m_buffer.begin(), m_buffer.end());
	}

	//------------------------------------------------------------------------------------------------------------------------
	const char* cstring_buffer::data() const
	{
		return m_buffer.data();
	}

} //- core