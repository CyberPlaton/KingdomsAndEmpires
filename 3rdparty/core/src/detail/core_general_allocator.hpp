#pragma once
#include "core_allocator.hpp"

namespace core
{
	//- General-purpose allocator utilizing CORE_MALLOC. Capacity and size stay always the same; Requires some overhead
	//- in memory and CPU time for management.
	//------------------------------------------------------------------------------------------------------------------------
	class cgeneral_allocator final : public iallocator
	{
	public:
		cgeneral_allocator(const char* name = iallocator::C_DEFAULT_ALLOCATOR_NAME);
		~cgeneral_allocator();

		void* allocate(uint64_t size, uint64_t alignment = iallocator::C_ALIGNMENT) override final;
		void deallocate(void* ptr) override final;

		uint64_t allocations_count() { return m_allocations; }
		uint64_t deallocations_count() { return m_deallocations; }

	private:
#if PROFILE_ENABLE
		umap_t<uint64_t, uint64_t> m_pointers;
#endif
		uint64_t m_allocations;
		uint64_t m_deallocations;
	};

} //- core