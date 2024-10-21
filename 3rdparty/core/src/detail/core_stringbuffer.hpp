#pragma once
#include "core_config.hpp"

namespace core
{
	namespace detail
	{
		constexpr auto C_STRING_BUFFER_SIZE_DEFAULT = 32;

	} //- detail

	//- Class wrapping fmt memory buffer. Useful for constructing big strings.
	//------------------------------------------------------------------------------------------------------------------------
	class cstring_buffer final
	{
	public:
		cstring_buffer() = default;
		~cstring_buffer();

		cstring_buffer& write(const char* text);

		cstring_buffer& write(stringview_t text);

		cstring_buffer& write(const string_t& text);

		template<typename... ARGS>
		cstring_buffer& write(const char* text, ARGS&&... args);

		void reserve(uint64_t n);
		void resize(uint64_t n);
		uint64_t size() const;
		bool empty() const;
		void clear();
		const char* data() const;
		const char* c_str();
		string_t string();

		inline auto begin() noexcept { return m_buffer.begin(); }
		inline auto end() noexcept { return m_buffer.end(); }

	private:
		fmt::basic_memory_buffer<char, detail::C_STRING_BUFFER_SIZE_DEFAULT> m_buffer;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename... ARGS>
	cstring_buffer& core::cstring_buffer::write(const char* text, ARGS&&... args)
	{
		fmt::detail::vformat_to(m_buffer, fmt::detail::to_string_view(text), fmt::make_format_args(args...));
		return *this;
	}

} //- core