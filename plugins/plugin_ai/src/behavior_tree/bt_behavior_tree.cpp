#include "bt_behavior_tree.hpp"

namespace ai
{
	namespace bt
	{
		namespace
		{
			static tree_id_t s_unique_tree_id = 0;

			//------------------------------------------------------------------------------------------------------------------------
			static tree_id_t generate_tree_id()
			{
				return ++s_unique_tree_id;
			}

		} //- unnamed

		//------------------------------------------------------------------------------------------------------------------------
		cbehavior_tree::cbehavior_tree(stringview_t name) :
			m_name(name), m_id(generate_tree_id()), m_next_id(0)
		{
			emplace<croot>();
		}

		//------------------------------------------------------------------------------------------------------------------------
		ai::bt::node_id_t cbehavior_tree::generate_node_id()
		{
			return m_next_id++;
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool cbehavior_tree::attach_node_to(node_id_t id, node_id_t parent)
		{
			if (auto it = algorithm::find_if(m_nodes.begin(), m_nodes.end(),
				[id = id, parent = parent](const rttr::variant& node)
				{
					if (const auto& id_func = node.get_type().get_method(C_ID_FUNCTION_NAME.data()); id_func.is_valid() &&
						id_func.invoke(node).convert<node_id_t>() == parent)
					{
						return true;
					}
					return false;
				});
				it != m_nodes.end())
			{
				auto& parent_node = *it;

				//- optimizing here is not really required, emplacing children will not be done on a per-frame basis
				if (const auto& m = parent_node.get_type().get_method(C_EMPLACE_CHILD_FUNCTION_NAME.data()); m.is_valid())
				{
					m.invoke(parent_node, id);
					return true;
				}
			}
			return false;
		}

		//------------------------------------------------------------------------------------------------------------------------
		ai::bt::node_id_t ibehavior_tree_context_holder::id() const
		{
			return m_id;
		}

		//------------------------------------------------------------------------------------------------------------------------
		ai::bt::cbehavior_tree& ibehavior_tree_context_holder::tree()
		{
			return m_context;
		}

		//------------------------------------------------------------------------------------------------------------------------
		const ai::bt::cbehavior_tree& ibehavior_tree_context_holder::tree() const
		{
			return m_context;
		}

		//------------------------------------------------------------------------------------------------------------------------
		ai::bt::tick_result cfallback::do_tick()
		{
			logging::log_debug("cfallback do_tick");

			for (const auto& i : m_children)
			{
				if (do_tick_child(i) == tick_result_ok)
				{
					return tick_result_ok;
				}
			}
			return tick_result_fail;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cfallback::emplace_child(node_id_t id)
		{
			logging::log_debug(fmt::format("cfallback adding child '{}'", id));

			m_children.push_back(id);
		}

		//------------------------------------------------------------------------------------------------------------------------
		ai::bt::tick_result csequence::do_tick()
		{
			logging::log_debug("csequence do_tick");

			for (const auto& i : m_children)
			{
				if (do_tick_child(i) == tick_result_fail)
				{
					return tick_result_fail;
				}
			}
			return tick_result_ok;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void croot::do_tick()
		{
			logging::log_debug("croot do_tick");

			for (const auto& i : m_children)
			{
				do_tick_child(i);
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		void croot::emplace_child(node_id_t id)
		{
			logging::log_debug(fmt::format("croot adding child '{}'", id));

			CORE_ASSERT(id > 0, "Invalid operation. Trying to attach root to itself is not allowed");

			m_children.push_back(id);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void csequence::emplace_child(node_id_t id)
		{
			logging::log_debug(fmt::format("csequence adding child '{}'", id));

			m_children.push_back(id);
		}

		//------------------------------------------------------------------------------------------------------------------------
		ai::bt::tick_result caction::do_tick()
		{
			logging::log_debug("caction do_tick");

			return tick_result_ok;
		}

		//------------------------------------------------------------------------------------------------------------------------
		ai::bt::tick_result ccondition::do_tick()
		{
			logging::log_debug("ccondition do_tick");

			return tick_result_ok;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cbehavior_tree::tick()
		{
			CORE_NAMED_ZONE("cbehavior_tree::tick");

			const auto& root = m_nodes[0];

			const auto& method = root.get_type().get_method(C_TICK_FUNCTION_NAME.data());

			method.invoke(root);
		}

		//------------------------------------------------------------------------------------------------------------------------
		ai::bt::tick_result cbehavior_tree::tick_node(uint32_t i)
		{
			CORE_ASSERT(i < m_nodes.size(), "Invalid operation. Child index out of bound");

			const auto& node = m_nodes[i];

			const auto& method = node.get_type().get_method(C_TICK_FUNCTION_NAME.data());

			const auto valid = method.is_valid();

			//- It is most likely that the nodes are correctly setup, so this is a reasonable optimization.
			//- However care must be taken to not forget and setup those node classes, thats why we want to explicitly throw.
			if (CORE_LIKELY(valid))
			{
				return method.invoke(node).convert<tick_result>();
			}

			CORE_ASSERT(false, "Invalid operation. 'tick' function was not configured for node");

			return tick_result_fail;
		}

		//------------------------------------------------------------------------------------------------------------------------
		ai::bt::tree_id_t cbehavior_tree::id() const
		{
			return m_id;
		}

		//------------------------------------------------------------------------------------------------------------------------
		stringview_t cbehavior_tree::name() const
		{
			return m_name.data();
		}

		//------------------------------------------------------------------------------------------------------------------------
		ai::bt::tick_result caction_logg::do_tick()
		{
			logging::log_debug(fmt::format("[Behavior Tree '{} (#{})'] '{}'", tree().name(), tree().id(), m_message));

			return tick_result_ok;
		}

	} //- bt

} //- ai