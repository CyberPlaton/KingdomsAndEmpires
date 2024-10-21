#pragma once
#include "core_config.hpp"

namespace core
{
	namespace detail
	{
		constexpr uint64_t C_STRING_SSO_ALIGNMENT = 16;
		constexpr uint64_t C_STRING_SSO_SIZE_DEFAULT = 16;
		constexpr bool C_STRING_USE_SIMD = true;
		constexpr auto C_STRING_CAPACITY_INCREASE_RATIO = 3;

	} //- detail

	//------------------------------------------------------------------------------------------------------------------------
	template<uint64_t SSO>
	class
#if CORE_PLATFORM_WINDOWS
		__declspec(align(SSO))
#else
		__attribute__((aligned(SSO)))
#endif
		istring
	{
	public:
		virtual ~istring() = default;
	};

	//- Class replacing std::string inspired by https://github.com/RobloxResearch/SIMDString
	//- 1. SSO (Small String Optimization). Does not require memory allocations for smaller sized strings
	//------------------------------------------------------------------------------------------------------------------------
	class cstring final : public istring<detail::C_STRING_SSO_SIZE_DEFAULT>
	{
	public:
		static constexpr auto npos = MAX(uint64_t);

		static uint64_t memory_used();
		static uint64_t peak_memory_used();
		static uint64_t allocations_count();
		static uint64_t deallocations_count();

		cstring();
		~cstring();

		cstring(const cstring& other);
		cstring& operator=(const cstring& other) noexcept;

		cstring(cstring&& other);
		cstring& operator=(cstring&& other) noexcept;

		//- Initialize string with given size and fill with given character
		explicit cstring(uint64_t n, char c = '\0');

		//- Initialize string from given string or a substring there of
		explicit cstring(const cstring& string, uint64_t offset = 0, uint64_t count = npos);
		explicit cstring(const char* string, uint64_t offset = 0, uint64_t count = npos);

		const char* c_str() const noexcept;
		const char* data() const noexcept;
		const char* data() noexcept;

		uint64_t length() const;
		uint64_t size() const { return length(); }
		uint64_t capacity() const;

		//- Modifying operations
		void append(const char* first, const char* last);
		void assign(const char* first, uint64_t count);
		void push_back(const char c);
		char pop_back();
		void reserve(uint64_t count);
		void resize(uint64_t count);
		void clear();

	private:
		static_assert(detail::C_STRING_SSO_SIZE_DEFAULT >= detail::C_STRING_SSO_ALIGNMENT &&
			detail::C_STRING_SSO_SIZE_DEFAULT % detail::C_STRING_SSO_ALIGNMENT == 0, "Invalid operation. SSO size must be a multiple of SSO alignment!");

		char m_buffer[detail::C_STRING_SSO_SIZE_DEFAULT + 1] = { '\0' };//- Stack memory string for smaller sized strings
		char* m_first = nullptr;										//- Empty unless stack memory is not enough and we allocated extra memory for string
		char* m_last = nullptr;											//- 
		uint64_t m_capacity = detail::C_STRING_SSO_SIZE_DEFAULT;		//- Reserved memory when allocated extra memory or zero if not

	private:
		inline bool in_heap() const { return m_capacity > detail::C_STRING_SSO_SIZE_DEFAULT || m_first != &m_buffer[0]; }

		void memcopy(void* dest, const void* source, uint64_t size);
		void memset(void* dest, char value, uint64_t size);
	};

} //- core