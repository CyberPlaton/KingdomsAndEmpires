#include "core_memory.hpp"

namespace core
{
	//------------------------------------------------------------------------------------------------------------------------
	memory_ref_t cmemory::make_ref(byte_t* data, unsigned size)
	{
		return std::make_shared<cmemory>(data, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	memory_ref_t cmemory::make_ref(unsigned capacity, const bool resize /*= false*/)
	{
		return std::make_shared<cmemory>(capacity, resize);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmemory::cmemory(byte_t* data, unsigned size)
	{
		const auto n = SCAST(size_t, size + 1);

		m_data.resize(n);

		//- Copy data, freeing incoming data is not our responsibility
		std::memcpy(m_data.data(), data, n);

		m_data[size] = '\0';
	}

	//- Allocate a piece of memory with requested size
	//------------------------------------------------------------------------------------------------------------------------
	cmemory::cmemory(unsigned capacity, const bool resize /*= false*/)
	{
		const auto n = capacity + 1;

		if (resize)
		{
			m_data.resize(SCAST(size_t, n));
			m_data[capacity] = '\0';
		}
		else
		{
			m_data.reserve(SCAST(size_t, n));
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmemory::~cmemory()
	{
		m_data.clear();
	}

} //- core