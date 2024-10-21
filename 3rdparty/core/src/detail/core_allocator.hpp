#pragma once
#include "core_config.hpp"

namespace core
{
	namespace detail
	{
		bool is_power_of_2(uint64_t n);
		uint64_t next_power_of_2(uint64_t n);
		uint64_t memloc(void* m);
		uint64_t memloc_index(void* m, void* start, uint64_t object_size);
		uint64_t align_address_forward(uint64_t address, uint64_t alignment);

		//- Calculate amount of bytes required for the given address to be aligned including the size of the header.
		//- Note: alignment must be greater than 0.
		//------------------------------------------------------------------------------------------------------------------------
		template<typename THeader>
		inline uint64_t align_address_forward_with_header(uint64_t address, uint64_t alignment)
		{
			CORE_ASSERT(alignment > 0, "Invalid operation. Alignment must be greater than 0!");

			auto padding = align_address_forward(address, alignment);
			auto required_space = sizeof(THeader);

			if (padding < required_space)
			{
				//- the header does not fit, so we calculate next aligned address in which it fits
				required_space -= padding;

				if (required_space % alignment > 0)
				{
					padding += alignment * (1u + (required_space / alignment));
				}
				else
				{
					padding += alignment * (required_space / alignment);
				}
			}

			return padding;
		}

	} //- detail

	//- Interface class for an allocator implementation.
	//------------------------------------------------------------------------------------------------------------------------
	class CORE_NO_VTABLE iallocator
	{
	public:
		static constexpr const char* C_DEFAULT_ALLOCATOR_NAME = "core::iallocator";
		static constexpr uint64_t C_ALIGNMENT = 8;

		//- in-place construct an object. Should be used for allocated objects that need additional construction.
		template<typename TObjectType, typename... ARGS>
		static TObjectType* construct(TObjectType* pointer, ARGS&&... args)
		{
			return new(SCAST(void*, pointer)) TObjectType(std::forward<ARGS>(args)...);
		}

		//- destruct object that requires destruction.
		template<typename TObjectType>
		static void destroy(TObjectType* pointer)
		{
			CORE_ASSERT(pointer, "Invalid operation. Pointer must be a valid object!");
			pointer->~TObjectType();
		}

		iallocator(const char* name = C_DEFAULT_ALLOCATOR_NAME);
		virtual ~iallocator() = 0;
		virtual void* allocate(uint64_t size, uint64_t alignment = iallocator::C_ALIGNMENT) = 0;
		virtual void deallocate(void* ptr) = 0;

		uint64_t capacity() const;
		uint64_t used() const;
		uint64_t peak_used() const;
		const char* name() const;

	protected:
		uint64_t m_total_size = 0;		//- amount of available memory
		uint64_t m_used_size = 0;		//- current bytes in use
		uint64_t m_peak_used_size = 0;	//- biggeest amount of memory used
		const char* m_name = nullptr;	//- allocator name used for debug purposes

	protected:
		void init(uint64_t size);
		void track_allocation(int64_t allocation_size, uint64_t padding = 0);
	};

} //- core