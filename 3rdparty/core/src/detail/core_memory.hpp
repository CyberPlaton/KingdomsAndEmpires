#pragma once
#include "core_config.hpp"

namespace core
{
	//- Memory storage that can be copied around using a shared pointer, when going out of scope for all shared pointers
	//- will be free using the provided release callback. At all times the actual memory is not duplicated.
	//- Note: when initialized with an existing piece of memory, that piece is copied and the original memory is freed.
	//------------------------------------------------------------------------------------------------------------------------
	class cmemory final :
		private cnon_copyable,
		private cnon_movable
	{
	public:
		static memory_ref_t make_ref(byte_t* data, unsigned size);
		static memory_ref_t make_ref(unsigned capacity, const bool resize = false);

		//- Constructors must be declared as public, otherwise std::make_shared cannot access them.
		cmemory(unsigned capacity, const bool resize = false);
		cmemory(byte_t* data, unsigned size);
		~cmemory();

		inline byte_t* data() { return m_data.data(); }
		inline const byte_t* data() const { return m_data.data(); }
		inline unsigned size() const { return SCAST(unsigned, m_data.size()); }
		inline bool empty() const { return size() == 0; }
		inline auto begin() { return m_data.begin(); }
		inline auto end() { return m_data.end(); }
		inline void resize(unsigned n) { m_data.resize(SCAST(size_t, n)); }

		//- Take ownership of the data
		[[nodiscard]] vector_t<byte_t>&& take() { return std::move(m_data); }

	private:
		vector_t<byte_t> m_data;
	};

} //- core