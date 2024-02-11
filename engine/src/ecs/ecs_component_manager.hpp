#pragma once
#include <core.h>

namespace ecs
{
	namespace detail
	{
		constexpr uint64_t C_STARTING_COMPONENT_COUNT = 1024;

		class ipool
		{
		public:
		};

		//- Note: you have to deallocate memory using the shutdown function
		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		class cdynamic_pool : public ipool
		{
		public:
			cdynamic_pool();
			~cdynamic_pool() = default;

			bool init(uint64_t count, uint64_t alignment = 0);

			void shutdown();

			void reset();

			TType* create(uint64_t* index_out);

			TType* replace(uint64_t index);

			void destroy(uint64_t index);

			TType* begin();

			TType* advance(TType* object);

			const TType* find(uint64_t index) const;

			TType* modify(uint64_t index);

			uint64_t size() const;

			uint64_t capacity() const;


		private:
			vector_t<bool> m_initialized_bit;

			std::stack<unsigned> m_fragmentation_indices;

			uint64_t m_top;

			uint64_t m_size;

			uint64_t m_capacity;

			uint64_t m_alignment;

			void* m_start;

			void* m_end;

		private:
			void resize(uint64_t count, uint64_t alignment);
			TType* unsafe(uint64_t index);
		};

	} //- detail

	//------------------------------------------------------------------------------------------------------------------------
	class ccomponent_manager
	{
	public:

		template<class TComponent>
		TComponent& find(uint64_t entity)
		{
			auto id = rttr::type::get<TComponent>().get_id();
			auto it = stl::find(m_entity_component_lookup, m_entity_component_lookup, entity);

			if (it == m_entity_component_lookup.end())
			{
				//- add and return
			}

			for (const auto& pair : it->second)
			{
				if (pair.first == id)
				{
					auto& pool = component_pool<TComponent>(m_component_pool[id]);

					return pool.find(pair.second);
				}
			}
		}


		template<class TComponent>
		TComponent& get(uint64_t index)
		{
			auto id = rttr::type::get<TComponent>().get_id();

			auto& pool = component_pool<TComponent>(m_component_pool[id]);

			return *pool.modify(index);
		}

		template<class TComponent>
		TComponent& add(uint64_t* index_out)
		{
			auto id = rttr::type::get<TComponent>().get_id();

			if (m_component_pool.find(id) == m_component_pool.end())
			{
				auto p = std::make_shared<detail::cdynamic_pool<TComponent>>();

				p->init(detail::C_STARTING_COMPONENT_COUNT);

				m_component_pool[id] = std::move(p);
			}

			auto& pool = component_pool<TComponent>(m_component_pool[id]);

			return *pool.create(index_out);
		}

	private:
		umap_t<rttr::type::type_id, ref_t<detail::ipool>> m_component_pool;
		umap_t<uint64_t, vector_t<std::pair<rttr::type::type_id, uint64_t>>> m_entity_component_lookup;

	private:
		template<class TComponent>
		detail::cdynamic_pool<TComponent>& component_pool(const ref_t<detail::ipool>& pointer)
		{
			return *reinterpret_cast<detail::cdynamic_pool<TComponent>*>(pointer.get());
		}
	};

} //- ecs

namespace ecs
{
	namespace detail
	{
		namespace
		{
			//------------------------------------------------------------------------------------------------------------------------
			inline static uint64_t next_power_of_2(uint64_t n)
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
			inline static uint64_t memloc(void* m)
			{
				return reinterpret_cast<uint64_t>(m);
			}

			//------------------------------------------------------------------------------------------------------------------------
			inline static uint64_t memloc_index(void* m, void* start, uint64_t object_size)
			{
				return (reinterpret_cast<uint64_t>(m) - reinterpret_cast<uint64_t>(start)) / object_size;
			}

		} //- unnamed

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		ecs::detail::cdynamic_pool<TType>::cdynamic_pool() :
			m_top(0), m_size(0), m_capacity(0), m_alignment(0), m_start(nullptr), m_end(nullptr)
		{
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		TType* ecs::detail::cdynamic_pool<TType>::unsafe(uint64_t index)
		{
			return reinterpret_cast<TType*>(reinterpret_cast<void*>((reinterpret_cast<uint64_t>(m_start) + index * sizeof(TType))));
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		void ecs::detail::cdynamic_pool<TType>::resize(uint64_t count, uint64_t alignment)
		{
			auto prev_start = m_start;
			auto prev_count = m_size;
			auto prev_align = m_alignment;

			m_start = mi_aligned_alloc(alignment, count * sizeof(TType));

			m_end = reinterpret_cast<void*>(memloc(m_start) + count * SCAST(uint64_t, sizeof(TType)));

			m_capacity = count * SCAST(uint64_t, sizeof(TType));

			m_alignment = alignment;

			m_initialized_bit.resize(count, false);

			std::memmove(m_start, prev_start, prev_count * sizeof(TType));

			mi_free_size_aligned(prev_start, prev_count, prev_align);
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		uint64_t ecs::detail::cdynamic_pool<TType>::capacity() const
		{
			return m_capacity;
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		uint64_t ecs::detail::cdynamic_pool<TType>::size() const
		{
			return m_size;
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		TType* ecs::detail::cdynamic_pool<TType>::modify(uint64_t index)
		{
			return unsafe(index);
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		const TType* ecs::detail::cdynamic_pool<TType>::find(uint64_t index) const
		{
			return unsafe(index);
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		TType* ecs::detail::cdynamic_pool<TType>::advance(TType* object)
		{
			auto next = reinterpret_cast<void*>(memloc(object) + sizeof(TType));

			while (next <= m_end && !m_initialized_bit[memloc_index(next, m_start, sizeof(TType))])
			{
				next += sizeof(TType);
			}

			next <= m_end ? return reinterpret_cast<TType*>(next) : return nullptr;
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		TType* ecs::detail::cdynamic_pool<TType>::begin()
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
		void ecs::detail::cdynamic_pool<TType>::destroy(uint64_t index)
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
		TType* ecs::detail::cdynamic_pool<TType>::replace(uint64_t index)
		{
			TType* object = unsafe(index);

			object->~TType();

			object = new (object) TType;

			return object;
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		TType* ecs::detail::cdynamic_pool<TType>::create(uint64_t* index_out)
		{
			if (size() == capacity())
			{
				resize(m_size * 2, m_alignment);
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

			m_initialized_bit[index] =  true;
			++m_size;

			*index_out = index;

			return object;
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		void ecs::detail::cdynamic_pool<TType>::reset()
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
		void ecs::detail::cdynamic_pool<TType>::shutdown()
		{
			auto* object = begin();
			while (object)
			{
				object->~TType();

				object = advance(object);
			}

			mi_free_size_aligned(m_start, m_size, m_alignment);
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		bool ecs::detail::cdynamic_pool<TType>::init(uint64_t count, uint64_t alignment /*= 0*/)
		{
			auto __alignment = alignment == 0 ? next_power_of_2(alignof(TType)) : alignment;

			m_start = mi_aligned_alloc(__alignment, sizeof(TType) * count);

			m_end = reinterpret_cast<void*>(memloc(m_start) + count * SCAST(uint64_t, sizeof(TType)));

			m_capacity = count * SCAST(uint64_t, sizeof(TType));

			m_alignment = __alignment;

			m_initialized_bit.resize(count, false);

			return true;
		}

	} //- detail

} //- ecs