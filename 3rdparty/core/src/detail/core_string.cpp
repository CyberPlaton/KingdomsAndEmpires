#include "core_string.hpp"
#include "core_general_allocator.hpp"

namespace core
{
	namespace
	{
#if defined(__i386__) || defined(_M_IX86) || defined(__x86_64__) || defined(_M_X64)
#define SSEx64 1
#include <immintrin.h>
		using m128i_t = __m128i;
#elif defined(__ARM_NEON) || defined(__arm__) || defined(_M_ARM)
#define SSEx64 0
#include <arm_neon.h>
		using m128i_t = uint64x2_t;
#endif

		static cgeneral_allocator S_STRING_ALLOCATOR("string_allocator");

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	cstring::~cstring()
	{
		if (capacity() > detail::C_STRING_SSO_SIZE_DEFAULT)
		{
			S_STRING_ALLOCATOR.deallocate(m_first);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	uint64_t cstring::memory_used()
	{
		return S_STRING_ALLOCATOR.used();
	}

	//------------------------------------------------------------------------------------------------------------------------
	uint64_t cstring::peak_memory_used()
	{
		return S_STRING_ALLOCATOR.peak_used();
	}

	//------------------------------------------------------------------------------------------------------------------------
	uint64_t cstring::allocations_count()
	{
		return S_STRING_ALLOCATOR.allocations_count();
	}

	//------------------------------------------------------------------------------------------------------------------------
	uint64_t cstring::deallocations_count()
	{
		return S_STRING_ALLOCATOR.deallocations_count();
	}

	//------------------------------------------------------------------------------------------------------------------------
	cstring::cstring(const cstring& other) :
		m_first(&m_buffer[0]), m_last(&m_buffer[detail::C_STRING_SSO_SIZE_DEFAULT]), m_capacity(detail::C_STRING_SSO_SIZE_DEFAULT)
	{
		assign(other.m_first, other.length());
	}

	//------------------------------------------------------------------------------------------------------------------------
	cstring::cstring(cstring&& other) :
		m_first(&m_buffer[0]), m_last(&m_buffer[detail::C_STRING_SSO_SIZE_DEFAULT]), m_capacity(detail::C_STRING_SSO_SIZE_DEFAULT)
	{
		assign(other.m_first, other.length());
	}

	//------------------------------------------------------------------------------------------------------------------------
	cstring::cstring(const cstring& string, uint64_t offset /*= 0*/, uint64_t count /*= npos*/) :
		m_first(&m_buffer[0]), m_last(&m_buffer[detail::C_STRING_SSO_SIZE_DEFAULT]), m_capacity(detail::C_STRING_SSO_SIZE_DEFAULT)
	{
		assign(string.c_str() + offset, count == npos ? string.length() : count);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cstring::cstring() :
		m_first(&m_buffer[0]), m_last(&m_buffer[detail::C_STRING_SSO_SIZE_DEFAULT]), m_capacity(detail::C_STRING_SSO_SIZE_DEFAULT)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cstring::append(const char* first, const char* last)
	{
		const auto newsize = (uint64_t)((m_last - m_first) + (last - first) + 1);

		if (newsize > m_capacity)
		{
			reserve((newsize * detail::C_STRING_CAPACITY_INCREASE_RATIO) / 2);
		}

		for (; first != last; ++m_last, ++first)
		{
			*m_last = *first;
		}
		*m_last = '\0';
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cstring::assign(const char* first, uint64_t count)
	{
		clear();
		append(first, first + count);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cstring::clear()
	{
		resize(0);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cstring& cstring::operator=(cstring&& other) noexcept
	{
		append(other.m_first, other.m_first + other.length());
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cstring& cstring::operator=(const cstring& other) noexcept
	{
		append(other.m_first, other.m_first + other.length());
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cstring::cstring(uint64_t n, char c /*= '\0'*/) :
		m_first(&m_buffer[0]), m_last(&m_buffer[detail::C_STRING_SSO_SIZE_DEFAULT]), m_capacity(detail::C_STRING_SSO_SIZE_DEFAULT)
	{
		resize(n);

		for (char* it = m_first; it != m_last; ++it)
		{
			*it = c;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	cstring::cstring(const char* string, uint64_t offset /*= 0*/, uint64_t count /*= npos*/) :
		m_first(&m_buffer[0]), m_last(&m_buffer[detail::C_STRING_SSO_SIZE_DEFAULT]), m_capacity(detail::C_STRING_SSO_SIZE_DEFAULT)
	{
		assign(string + offset, count == npos ? strlen(string) : count);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cstring::reserve(uint64_t count)
	{
		const auto heap = in_heap();
		const auto size = length();
		char* newmem = (char*)S_STRING_ALLOCATOR.allocate(count);
		char* oldmem = m_first;

		if (size > detail::C_STRING_SSO_SIZE_DEFAULT)
		{
			memcopy(newmem, oldmem, size);
		}
		else if (size > 0)
		{
			std::memcpy(newmem, oldmem, size);
		}

		m_first = newmem;
		m_last = m_first + size;
		m_capacity = count;

		//- Free old memory used if we allocated anything
		if (heap)
		{
			S_STRING_ALLOCATOR.deallocate(oldmem);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cstring::resize(uint64_t count)
	{
		const auto prevsize = length();

		if (count > prevsize)
		{
			reserve(count);

			const char* end = m_first + count + 1;

			for (char* it = m_last; it < end; ++it)
			{
				*it = '\0';
			}
		}
		else if (m_last != m_first)
		{
			m_first[count] = '\0';
		}

		m_last = m_first + count;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cstring::memcopy(void* dest, const void* source, uint64_t size)
	{
		CORE_ASSERT(size > 0, "Invalid operation. Size must not be zero!");

		if constexpr (detail::C_STRING_USE_SIMD)
		{
			auto* p = reinterpret_cast<m128i_t*>(dest);
#if SSEx64
			auto* s = const_cast<m128i_t*>(reinterpret_cast<const m128i_t*>(source));
#else
			const auto* s = reinterpret_cast<const uint64_t*>(source);
#endif

			uint64_t i = size / detail::C_STRING_SSO_SIZE_DEFAULT;

			CORE_ASSERT(i, "Invalid operation. Either size to copy or SSO size is invalid!");

			while (i--)
			{
#if SSEx64
				p[i] = _mm_load_si128(s + i);
#else
				p[i] = vld1q_u64(s + (i * 2));
#endif
			}
		}
		else
		{
			std::memcpy(dest, source, size);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cstring::memset(void* dest, char value, uint64_t size)
	{
		std::memset(dest, SCAST(int, value), size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	const char* cstring::c_str() const noexcept
	{
		return m_first;
	}

	//------------------------------------------------------------------------------------------------------------------------
	const char* cstring::data() const noexcept
	{
		return m_first;
	}

	//------------------------------------------------------------------------------------------------------------------------
	const char* cstring::data() noexcept
	{
		return m_first;
	}

	//------------------------------------------------------------------------------------------------------------------------
	uint64_t cstring::length() const
	{
		return m_last - m_first;
	}

	//------------------------------------------------------------------------------------------------------------------------
	uint64_t cstring::capacity() const
	{
		return m_capacity;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cstring::push_back(const char c)
	{
		const auto newsize = length() + 1;

		if (newsize > m_capacity)
		{
			reserve(m_capacity * detail::C_STRING_CAPACITY_INCREASE_RATIO);
		}

		*m_last = c;

		++m_last;
	}

	//------------------------------------------------------------------------------------------------------------------------
	char cstring::pop_back()
	{
		const auto c = *m_last;

		*m_last = '\0';

		--m_last;

		return c;
	}

} //- core