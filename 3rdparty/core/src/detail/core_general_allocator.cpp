#include "core_general_allocator.hpp"

namespace core
{
	//------------------------------------------------------------------------------------------------------------------------
	cgeneral_allocator::cgeneral_allocator(const char* name /*= iallocator::C_DEFAULT_ALLOCATOR_NAME*/) :
		iallocator(name), m_allocations(0), m_deallocations(0)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cgeneral_allocator::~cgeneral_allocator()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void* cgeneral_allocator::allocate(uint64_t size, uint64_t alignment /*= iallocator::C_ALIGNMENT*/)
	{
		const auto mem = CORE_MALLOC(size);

#if PROFILE_ENABLE
		track_allocation(size);
		m_pointers[reinterpret_cast<uint64_t>(mem)] = size;
		++m_allocations;
#endif

		return mem;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cgeneral_allocator::deallocate(void* ptr)
	{
		CORE_FREE(ptr);

#if PROFILE_ENABLE
		track_allocation(-SCAST(int64_t, m_pointers.at(reinterpret_cast<uint64_t>(ptr))));
		m_pointers.erase(reinterpret_cast<uint64_t>(ptr));
		++m_deallocations;
#endif
	}

} //- core