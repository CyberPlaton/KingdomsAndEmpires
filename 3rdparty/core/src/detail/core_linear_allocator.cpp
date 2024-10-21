#include "core_linear_allocator.hpp"

namespace core
{
	//------------------------------------------------------------------------------------------------------------------------
	clinear_allocator::clinear_allocator(uint64_t size) :
		m_offset(0), m_memory(nullptr)
	{
		//- real allocation
		m_memory = CORE_MALLOC(size);

		init(size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	clinear_allocator::~clinear_allocator()
	{
		//- real deallocation
		CORE_FREE(m_memory);

		m_memory = nullptr;
		m_offset = 0;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void* clinear_allocator::allocate(uint64_t size, uint64_t alignment /*= iallocator::C_ALIGNMENT*/)
	{
		CORE_ASSERT(detail::is_power_of_2(alignment), "Invalid operation. Alignment must be 0 or a power of 2!");

		uint64_t padding = 0;

		const auto current = RCAST(uint64_t, m_memory) + m_offset;

		//- check if we need to align memory
		if (alignment > 0 && m_offset % alignment != 0)
		{
			padding = detail::align_address_forward(current, alignment);
		}

		if ((m_offset + padding + size) < capacity())
		{
			m_offset = (padding + size);
			const auto result = current + padding;

			track_allocation(size, padding);

			return RCAST(void*, result);
		}

		//- No memory left for use
		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void clinear_allocator::deallocate(void* ptr)
	{
		//- noop
	}

} //- core