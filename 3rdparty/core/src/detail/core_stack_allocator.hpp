#pragma once
#include "core_allocator.hpp"

namespace core
{
	//- Allocator allowing for push and pop operations. Performs allocations in a linear manner in a contiguous region of memory.
	//------------------------------------------------------------------------------------------------------------------------
	class cstack_allocator final : public iallocator
	{
	public:
		cstack_allocator(uint64_t size);
		~cstack_allocator();

		void* allocate(uint64_t size, uint64_t alignment = iallocator::C_ALIGNMENT) override final;
		void deallocate(void* ptr) override final;

	private:
		struct sheader
		{
			uint8_t m_padding = 0;
			uint64_t m_size = 0;
		};

		uint64_t m_offset;
		void* m_memory;
	};

} //- core