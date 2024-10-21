#pragma once
#include "core_allocator.hpp"

namespace core
{
	//- Allocator also known as an 'Arena'. Performs allocations in a linear manner in a contiguous region of memory.
	//- Note that it does not deallocate anything, it will free all used memory once out of scope.
	//------------------------------------------------------------------------------------------------------------------------
	class clinear_allocator final : public iallocator
	{
	public:
		clinear_allocator(uint64_t size);
		~clinear_allocator();

		void* allocate(uint64_t size, uint64_t alignment = iallocator::C_ALIGNMENT) override final;
		void deallocate(void* ptr) override final;

	private:
		uint64_t m_offset;
		void* m_memory;
	};

} //- core