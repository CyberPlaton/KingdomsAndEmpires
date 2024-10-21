#include "core_allocator.hpp"

namespace core
{
	namespace detail
	{
		//- Calculate amount of bytes required for the given address to be aligned.
		//------------------------------------------------------------------------------------------------------------------------
		uint64_t align_address_forward(uint64_t address, uint64_t alignment)
		{
			CORE_ASSERT(alignment > 0, "Invalid operation. Alignment must be greater than 0!");

			auto result = address;
			const auto modulo = address & (address - 1);

			if (modulo != 0)
			{
				result += alignment - modulo;
			}

			return result;
		}

		//- Note: returns true for 0 not to trigger anything
		//------------------------------------------------------------------------------------------------------------------------
		bool is_power_of_2(uint64_t n)
		{
			return n > 0 ? (n & (n - 1)) == 0 : true;
		}

		//------------------------------------------------------------------------------------------------------------------------
		uint64_t next_power_of_2(uint64_t n)
		{
			n--;
			n |= n >> 1;
			n |= n >> 2;
			n |= n >> 4;
			n |= n >> 8;
			n |= n >> 16;
			n++;
			return n;
		}

		//------------------------------------------------------------------------------------------------------------------------
		uint64_t memloc(void* m)
		{
			return reinterpret_cast<uint64_t>(m);
		}

		//------------------------------------------------------------------------------------------------------------------------
		uint64_t memloc_index(void* m, void* start, uint64_t object_size)
		{
			return (reinterpret_cast<uint64_t>(m) - reinterpret_cast<uint64_t>(start)) / object_size;
		}

	} //- detail

	//------------------------------------------------------------------------------------------------------------------------
	iallocator::~iallocator()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	uint64_t iallocator::capacity() const
	{
		return m_total_size;
	}

	//------------------------------------------------------------------------------------------------------------------------
	uint64_t iallocator::used() const
	{
		return m_used_size;
	}

	//------------------------------------------------------------------------------------------------------------------------
	uint64_t iallocator::peak_used() const
	{
		return m_peak_used_size;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void iallocator::init(uint64_t size)
	{
		m_total_size = size;
	}

	//- Note: this tracks a memory allocation or deallocation depending on given allocation size.
	//------------------------------------------------------------------------------------------------------------------------
	void iallocator::track_allocation(int64_t allocation_size, uint64_t padding /*= 0*/)
	{
		m_used_size += (allocation_size + padding);
		m_peak_used_size = glm::max(m_peak_used_size, m_used_size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	iallocator::iallocator(const char* name /*= C_DEFAULT_ALLOCATOR_NAME*/) :
		m_name(name)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	const char* iallocator::name() const
	{
		return m_name;
	}

} //- core