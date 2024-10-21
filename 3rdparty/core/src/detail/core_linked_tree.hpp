#pragma once
#include "core_config.hpp"
#include "core_dynamic_pool.hpp"

namespace core
{
	constexpr uint64_t C_LINKED_TREE_DEFAULT_CAPACITY = 512;

	//- TODO: still work-in-progress
	//------------------------------------------------------------------------------------------------------------------------
	template<class TType>
	class clinked_tree
	{
	public:
		struct snode : public TType
		{
			snode* m_parent = nullptr;
			vector_t<snode*> m_children;
		};

		using visitor_func_t = std::function<bool(snode*)>;

		clinked_tree(uint64_t size = C_LINKED_TREE_DEFAULT_CAPACITY)
		{
			m_pool.init(size);

			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(logging_verbosity_debug,
					fmt::format("Creating clinked_tree with reserved capacity of '{}({}KB)'",
						m_pool.capacity(), algorithm::bytes_to_kilobytes(m_pool.memory_reserved())));
			}
		}

		~clinked_tree()
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(logging_verbosity_debug,
					fmt::format("Destroying clinked_tree of size '{}({}KB)' and capacity '{}({}KB)'",
						m_pool.size(), algorithm::bytes_to_kilobytes(m_pool.memory_usage()),
						m_pool.capacity(), algorithm::bytes_to_kilobytes(m_pool.memory_reserved())));
			}

			m_pool.shutdown();
		}

		void visit_depth_first(visitor_func_t visitor)
		{
			if (!m_pool.empty())
			{
				std::stack<snode*> stack;
				stack.push(m_pool.begin());

				while (!stack.empty())
				{
					auto n = stack.top();
					stack.pop();

					if (!visitor(n)) { break; }

					for (const auto& k : n->m_children)
					{
						stack.push(k);
					}
				}
			}
		}

		template<typename TCallable>
		void visit_depth_first(TCallable&& lambda)
		{
			visit_depth_first(algorithm::convert_to_function(lambda));
		}

		snode* append_to(snode* node = nullptr, uint64_t* index_out = nullptr)
		{
			node = node ? node : &m_root;

			auto* n = m_pool.create(index_out);

			n->m_parent = node;
			node->m_children.push_back(n);

			return n;
		}

		const snode* find(uint64_t index) const
		{
			return m_pool.find(index);
		}

		snode* modify(uint64_t index)
		{
			return m_pool.modify(index);
		}

		snode* root() const
		{
			return m_pool.begin();
		}

		bool empty() const { return m_pool.empty(); }
		uint64_t size() const { return m_pool.size(); }
		uint64_t capacity() const { return m_pool.capacity(); }

	private:
		cdynamic_pool<snode> m_pool;
		snode m_root;
	};

} //- core