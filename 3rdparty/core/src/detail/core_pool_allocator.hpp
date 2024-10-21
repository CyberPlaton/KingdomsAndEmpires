#pragma once
#include "core_allocator.hpp"

namespace core
{
	//- Allocator managing a pool of fixed-size memory blocks.
	//- Note: does not use alignment and size for allocation, they are left for compatability.
	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	class cpool_allocator final : public iallocator
	{
	public:
		cpool_allocator(uint64_t object_count);
		~cpool_allocator();

		void* allocate(uint64_t size, uint64_t alignment = iallocator::C_ALIGNMENT) override final;
		void deallocate(void* ptr) override final;

		TType* allocate_object();

		template<typename... ARGS>
		TType* allocate_object_with(ARGS&&... args);

		void deallocate(TType* object);

	private:
		stack_t<void*> m_free_list;
		int64_t m_type_size;
		void* m_memory;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	template<typename... ARGS>
	TType* core::cpool_allocator<TType>::allocate_object_with(ARGS&&... args)
	{
		TType* object = (TType*)allocate(0);

		return iallocator::construct(object, std::forward<ARGS>(args)...);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	void core::cpool_allocator<TType>::deallocate(TType* object)
	{
		iallocator::destroy(object);

		deallocate((void*)object);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	TType* core::cpool_allocator<TType>::allocate_object()
	{
		TType* object = (TType*)allocate(0);

		return iallocator::construct(object);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	core::cpool_allocator<TType>::cpool_allocator(uint64_t object_count) :
		m_memory(nullptr), m_type_size(SCAST(unsigned, sizeof(TType)))
	{
		const auto size = m_type_size * object_count;

		//- real allocation
		m_memory = CORE_MALLOC(size);

		init(size);

		//- divide mapped memory up into usable blocks of memory chunks
		for (uint64_t i = 0; i < object_count; ++i)
		{
			uint64_t p = RCAST(uint64_t, m_memory) + i * m_type_size;

			void* pointer = RCAST(void*, p);

			m_free_list.push(pointer);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	core::cpool_allocator<TType>::~cpool_allocator()
	{
		//- real deallocation
		CORE_ASSERT(m_used_size == 0, "Invalid operation. Objects were not deallocated and are still in use!");

		CORE_FREE(m_memory);

		m_memory = nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	void* core::cpool_allocator<TType>::allocate(uint64_t /*size*/, uint64_t /*alignment*/ /*= iallocator::C_ALIGNMENT*/)
	{
		void* pointer = nullptr;

		if (!m_free_list.empty())
		{
			pointer = m_free_list.top();

			m_free_list.pop();

			track_allocation(m_type_size, 0);
		}

		return pointer;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	void core::cpool_allocator<TType>::deallocate(void* ptr)
	{
		m_free_list.push(ptr);

		track_allocation(-m_type_size, 0);
	}

} //- core