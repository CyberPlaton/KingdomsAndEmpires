#pragma once
#include "core_config.hpp"

namespace core
{
	namespace detail
	{
		//- utility to allow storing pointers to dynamic pools holding arbitrary template types
		//------------------------------------------------------------------------------------------------------------------------
		struct ipool {};

	} //- detail

	//- Resizable and memory friendly pool providing constant access time and index stability, meaning
	//- when an element is removed other indices stay valid, so that they could be used as handles.
	//- Note: you have to deallocate memory using the shutdown function
	//------------------------------------------------------------------------------------------------------------------------
	template<class TType>
	class cdynamic_pool : public ipool
	{
	public:
		cdynamic_pool();
		~cdynamic_pool() = default;

		bool init(uint64_t count);
		void shutdown();
		void reset();
		TType* create(uint64_t* index_out);
		template<typename... ARGS>
		TType* create(uint64_t* index_out, ARGS&&... args);
		TType* replace(uint64_t index);
		void destroy(uint64_t index);
		TType* begin();
		TType* advance(TType* object);
		const TType* find(uint64_t index) const;
		TType* modify(uint64_t index);
		uint64_t size() const;
		uint64_t capacity() const;
		bool empty() const;
		uint64_t memory_usage() const;
		uint64_t memory_reserved() const;

	private:
		vector_t<bool> m_initialized_bit;
		std::stack<unsigned> m_fragmentation_indices;
		uint64_t m_top;
		uint64_t m_size;
		uint64_t m_capacity;
		void* m_start;
		void* m_end;

	private:
		void resize(uint64_t count);
		TType* unsafe(uint64_t index);
		const TType* unsafe(uint64_t index) const;
		bool initialized_at_index(uint64_t index) const;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<class TType>
	core::cdynamic_pool<TType>::cdynamic_pool() :
		m_top(0), m_size(0), m_capacity(0), m_start(nullptr), m_end(nullptr)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TType>
	TType* core::cdynamic_pool<TType>::unsafe(uint64_t index)
	{
		return reinterpret_cast<TType*>(reinterpret_cast<void*>((reinterpret_cast<uint64_t>(m_start) + index * sizeof(TType))));
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TType>
	const TType* core::cdynamic_pool<TType>::unsafe(uint64_t index) const
	{
		return reinterpret_cast<TType*>(reinterpret_cast<void*>((reinterpret_cast<uint64_t>(m_start) + index * sizeof(TType))));
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TType>
	void core::cdynamic_pool<TType>::resize(uint64_t count)
	{
		auto prev_start = m_start;
		auto prev_count = m_size;

		m_start = CORE_MALLOC(count * sizeof(TType));

		m_end = reinterpret_cast<void*>(memloc(m_start) + count * SCAST(uint64_t, sizeof(TType)));

		m_capacity = count;

		m_initialized_bit.resize(count, false);

		std::memmove(m_start, prev_start, prev_count * sizeof(TType));

		CORE_FREEN(prev_start, prev_count);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TType>
	uint64_t core::cdynamic_pool<TType>::capacity() const
	{
		return m_capacity;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TType>
	uint64_t core::cdynamic_pool<TType>::memory_usage() const
	{
		return size() * sizeof(TType);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TType>
	uint64_t core::cdynamic_pool<TType>::memory_reserved() const
	{
		return capacity() * sizeof(TType);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TType>
	bool core::cdynamic_pool<TType>::empty() const
	{
		return m_size == 0;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TType>
	uint64_t core::cdynamic_pool<TType>::size() const
	{
		return m_size;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TType>
	TType* core::cdynamic_pool<TType>::modify(uint64_t index)
	{
		return unsafe(index);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TType>
	const TType* core::cdynamic_pool<TType>::find(uint64_t index) const
	{
		return unsafe(index);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TType>
	bool core::cdynamic_pool<TType>::initialized_at_index(uint64_t index) const
	{
		return  index < capacity() && m_initialized_bit[index];
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TType>
	TType* core::cdynamic_pool<TType>::advance(TType* object)
	{
		auto next = memloc(object) + sizeof(TType);

		while (next <= RCAST(uint64_t, m_end) && !initialized_at_index(memloc_index(RCAST(void*, next), m_start, sizeof(TType))))
		{
			next += sizeof(TType);
		}

		return next <= reinterpret_cast<uint64_t>(m_end) ? reinterpret_cast<TType*>(next) : nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TType>
	TType* core::cdynamic_pool<TType>::begin()
	{
		if (m_size == 0)
		{
			return nullptr;
		}

		if (!m_initialized_bit[0])
		{
			return advance(reinterpret_cast<TType*>(m_start));
		}

		return reinterpret_cast<TType*>(m_start);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TType>
	void core::cdynamic_pool<TType>::destroy(uint64_t index)
	{
		if (m_size > 0)
		{
			const auto fragmentation = index < m_size - 1;

			TType* object = unsafe(index);

			object->~TType();

			m_initialized_bit[index] = false;

			--m_size;

			if (fragmentation)
			{
				m_fragmentation_indices.push(index);
			}
			else
			{
				--m_top;
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TType>
	TType* core::cdynamic_pool<TType>::replace(uint64_t index)
	{
		TType* object = unsafe(index);

		object->~TType();

		object = new (object) TType;

		return object;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TType>
	template<typename... ARGS>
	TType* core::cdynamic_pool<TType>::create(uint64_t* index_out, ARGS&&... args)
	{
		if (size() == capacity())
		{
			resize(m_size * 2);
		}

		uint64_t index = 0;

		if (!m_fragmentation_indices.empty())
		{
			index = m_fragmentation_indices.top();

			m_fragmentation_indices.pop();
		}
		else
		{
			index = m_top++;
		}

		TType* object = unsafe(index);

		object = new (object) TType(std::forward<ARGS>(args)...);

		m_initialized_bit[index] = true;
		++m_size;

		if (index_out)
		{
			*index_out = index;
		}

		return object;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TType>
	TType* core::cdynamic_pool<TType>::create(uint64_t* index_out)
	{
		if (size() == capacity())
		{
			resize(m_size * 2);
		}

		uint64_t index = 0;

		if (!m_fragmentation_indices.empty())
		{
			index = m_fragmentation_indices.top();

			m_fragmentation_indices.pop();
		}
		else
		{
			index = m_top++;
		}

		TType* object = unsafe(index);

		object = new (object) TType;

		m_initialized_bit[index] = true;
		++m_size;

		if (index_out)
		{
			*index_out = index;
		}

		return object;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TType>
	void core::cdynamic_pool<TType>::reset()
	{
		auto i = 0;
		auto object = begin();
		while (i < m_size)
		{
			object->~TType();

			object = advance(object);

			++i;
		}

		std::memset(m_start, NULL, m_size);

		std::fill(m_initialized_bit.begin(), m_initialized_bit.end(), false);

		m_fragmentation_indices = std::stack<unsigned>();

		m_size = 0;
		m_capacity = 0;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TType>
	void core::cdynamic_pool<TType>::shutdown()
	{
		auto* object = begin();
		while (object)
		{
			object->~TType();

			object = advance(object);
		}

		CORE_FREEN(m_start, m_size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TType>
	bool core::cdynamic_pool<TType>::init(uint64_t count)
	{
		m_start = CORE_MALLOC(sizeof(TType) * count);

		m_end = reinterpret_cast<void*>(memloc(m_start) + count * SCAST(uint64_t, sizeof(TType)));

		m_capacity = count;

		m_initialized_bit.resize(count, false);

		return true;
	}

} //- core
