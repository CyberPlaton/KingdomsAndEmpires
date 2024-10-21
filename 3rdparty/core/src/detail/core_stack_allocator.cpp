#include "core_stack_allocator.hpp"

namespace core
{
	//------------------------------------------------------------------------------------------------------------------------
	cstack_allocator::cstack_allocator(uint64_t size) :
		m_offset(0), m_memory(nullptr)
	{
		//- real allocation
		m_memory = CORE_MALLOC(size);

		init(size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cstack_allocator::~cstack_allocator()
	{
		//- real deallocation
		CORE_ASSERT(m_used_size == 0, "Invalid operation. Objects were not deallocated and are still in use!");

		CORE_FREE(m_memory);

		m_memory = nullptr;
		m_offset = 0;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void* cstack_allocator::allocate(uint64_t size, uint64_t alignment /*= iallocator::C_ALIGNMENT*/)
	{
		CORE_ASSERT(detail::is_power_of_2(alignment), "Invalid operation. Alignment must be 0 or a power of 2!");

		uint64_t padding = 0;

		const auto current = RCAST(uint64_t, m_memory) + m_offset;

		padding = detail::align_address_forward_with_header<sheader>(size, alignment);

		if ((m_offset + padding + size) < capacity())
		{
			m_offset += (padding + size);
			const auto result = current + padding;
			const auto header_address = result - sizeof(sheader);

			sheader* header_pointer = RCAST(sheader*, header_address);
			header_pointer->m_padding = padding;
			header_pointer->m_size = size;

			track_allocation(size, padding);

			return RCAST(void*, result);
		}

		//- No memory left for use
		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cstack_allocator::deallocate(void* ptr)
	{
		const auto current = RCAST(uint64_t, ptr);
		const auto header_address = current - sizeof(sheader);
		const sheader* header_pointer{ RCAST(sheader*, header_address) };
		const auto padding = SCAST(uint64_t, header_pointer->m_padding);

		m_offset = current - padding - RCAST(uint64_t, m_memory);

		//- alignment padding is already included inside header pointer padding
		track_allocation(-(padding + header_pointer->m_size), 0);
	}

} //- core