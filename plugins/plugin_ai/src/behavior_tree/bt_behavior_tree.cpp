#include "bt_behavior_tree.hpp"

namespace ai
{
	namespace bt
	{
		namespace detail
		{
			constexpr stringview_t C_LAST_RUNNING_NODE_PROP_NAME = "last_running";
			static tree_id_t s_unique_tree_id = 0;

			//------------------------------------------------------------------------------------------------------------------------
			tree_id_t generate_tree_id()
			{
				return ++s_unique_tree_id;
			}

			//- We can not expect each function to be setup correctly here
			//------------------------------------------------------------------------------------------------------------------------
			tick_result invoke_node_function(const rttr::variant& node, stringview_t function)
			{
				if (const auto& m = node.get_type().get_method(function.data()); m.is_valid())
				{
					return m.invoke(node).convert<tick_result>();
				}
				return tick_result_fail;
			}

			//------------------------------------------------------------------------------------------------------------------------
			node_id_t node_id(const rttr::variant& node)
			{
				if (const auto& m = node.get_type().get_method(detail::C_ID_FUNCTION_NAME.data()); m.is_valid())
				{
					return m.invoke(node).convert<node_id_t>();
				}
			}

		} //- detail

		//------------------------------------------------------------------------------------------------------------------------
		tick_result ibehavior_tree_context_holder::do_tick_child(uint32_t i)
		{
			return tree().tick_node(i);
		}

		//------------------------------------------------------------------------------------------------------------------------
		cbehavior_tree::cbehavior_tree(stringview_t name) :
			m_name(name), m_id(detail::generate_tree_id()), m_next_id(0)
		{
			//- emplace root manually
			auto id = generate_node_id();

			auto& node = m_nodes.emplace_back(croot(*this, id));

			bb().emplace(detail::C_LAST_RUNNING_NODE_PROP_NAME, (node_id_t)invalid_handle_t);
		}

		//------------------------------------------------------------------------------------------------------------------------
		ai::bt::node_id_t cbehavior_tree::generate_node_id()
		{
			return m_next_id++;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cbehavior_tree::attach_node_to(node_id_t id, node_id_t parent)
		{
			const auto& parent_node = m_nodes.at(parent);

			if (const auto& m = parent_node.get_type().get_method(detail::C_EMPLACE_CHILD_FUNCTION_NAME.data()); m.is_valid())
			{
				m.invoke(parent_node, id);
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool cbehavior_tree::can_emplace_node(node_id_t parent)
		{
			//- emplacing a root node is not possible with this function
			if(m_next_id > 0)
			{
				//- make sure parent exists and is a type to take in a child
				if (const auto it = algorithm::find_if(m_nodes.begin(), m_nodes.end(),
					[&](const rttr::variant& node)
					{
						if (const auto& id_func = node.get_type().get_method(detail::C_ID_FUNCTION_NAME.data()); id_func.is_valid() &&
							id_func.invoke(node).convert<node_id_t>() == parent)
						{
							return true;
						}
						return false;
					});
					it != m_nodes.end())
				{
					auto& parent_node = *it;

					const auto& m = parent_node.get_type().get_method(detail::C_EMPLACE_CHILD_FUNCTION_NAME.data());

					if (m.is_valid())
					{
						return true;
					}

					logging::log_warn(fmt::format("[Behavior Tree '{} (#{})'] attaching node to parent '{}' failed, as the parent can not take in children",
						m_name.data(), m_id, parent));
				}
			}
			return false;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cbehavior_tree::store_last_running_node(node_id_t id)
		{
			bb().get<node_id_t>(detail::C_LAST_RUNNING_NODE_PROP_NAME) = id;
		}

		//------------------------------------------------------------------------------------------------------------------------
		ai::bt::node_id_t cbehavior_tree::last_running_node()
		{
			return bb().find<node_id_t>(detail::C_LAST_RUNNING_NODE_PROP_NAME);
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

			BT_EMIT_TICK_EVENT(tree().id(), id());

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
		void crandom_sequence::emplace_child(node_id_t id)
		{
			logging::log_debug(fmt::format("crandom_sequence adding child '{}'", id));

			m_children.push_back(id);
		}

		//------------------------------------------------------------------------------------------------------------------------
		ai::bt::tick_result crandom_sequence::do_tick()
		{
			logging::log_debug("crandom_sequence do_tick");

			BT_EMIT_TICK_EVENT(tree().id(), id());

			auto children = m_children;

			algorithm::shuffle(children.begin(), children.end(), 0);

			for (const auto& i : children)
			{
				if (do_tick_child(i) == tick_result_fail)
				{
					return tick_result_fail;
				}
			}
			return tick_result_ok;
		}

		//------------------------------------------------------------------------------------------------------------------------
		ai::bt::tick_result csequence::do_tick()
		{
			logging::log_debug("csequence do_tick");

			BT_EMIT_TICK_EVENT(tree().id(), id());

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

			BT_EMIT_TICK_EVENT(tree().id(), id());

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

			BT_EMIT_TICK_EVENT(tree().id(), id());

			return tick_result_ok;
		}

		//------------------------------------------------------------------------------------------------------------------------
		ai::bt::tick_result ccondition::do_tick()
		{
			logging::log_debug("ccondition do_tick");

			BT_EMIT_TICK_EVENT(tree().id(), id());

			return tick_result_ok;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cbehavior_tree::tick(tf::Taskflow& subflow, bool force_restart/* = false*/)
		{
			subflow.emplace([&]()
				{
					CORE_NAMED_ZONE("cbehavior_tree::tick");

					auto node_id = CORE_LIKELY(!force_restart) ? last_running_node() : 0;

					tick_node(node_id);
				});

			m_task = engine::cengine::service<engine::cthread_service>()->push_background_taskflow(subflow);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cbehavior_tree::tick(bool force_restart/* = false*/)
		{
			CORE_NAMED_ZONE("cbehavior_tree::tick");

			auto node_id = 0;

			if (CORE_LIKELY(!force_restart))
			{
				const auto id = last_running_node();

				if (id != invalid_handle_t)
				{
					node_id = id;
				}
			}

			tick_node(node_id);
		}

		//------------------------------------------------------------------------------------------------------------------------
		ai::bt::tick_result cbehavior_tree::tick_node(uint32_t i)
		{
			CORE_ASSERT(i < m_nodes.size(), "Invalid operation. Child index out of bound");

			const auto& node = m_nodes[i];

			const auto& method = node.get_type().get_method(detail::C_TICK_FUNCTION_NAME.data());

			const auto valid = method.is_valid();

			//- It is most likely that the nodes are correctly setup, so this is a reasonable optimization.
			//- However care must be taken to not forget and setup those node classes, thats why we want to explicitly throw.
			if (CORE_LIKELY(valid))
			{
				store_last_running_node(detail::node_id(node));

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
		core::future_status cbehavior_tree::status() const
		{
			using namespace std::chrono_literals;

			if (m_task.valid())
			{
				switch (m_task.wait_for(0us))
				{
				case std::future_status::ready: { return core::future_status_ready; }
				case std::future_status::deferred: { return core::future_status_deferred; }
				default:
				case std::future_status::timeout: { return core::future_status_pending; }
				}
			}
			return core::future_status_none;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cbehavior_tree::cancel()
		{
			if (m_task.valid())
			{
				m_task.cancel();
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		ai::bt::tick_result caction_logg::do_tick()
		{
			logging::log_debug(fmt::format("[Behavior Tree '{} (#{})'] '{}'", tree().name(), tree().id(), m_message));

			BT_EMIT_TICK_EVENT(tree().id(), id());

			return tick_result_ok;
		}

		//------------------------------------------------------------------------------------------------------------------------
		ai::bt::tick_result cdecorator::do_tick()
		{
			logging::log_debug("cdecorator do_tick");

			BT_EMIT_TICK_EVENT(tree().id(), id());

			return detail::invoke_node_function(m_child, detail::C_TICK_FUNCTION_NAME.data());
		}

		//------------------------------------------------------------------------------------------------------------------------
		ai::bt::tick_result cdecorator_inverter::do_tick()
		{
			logging::log_debug("cdecorator_inverter do_tick");

			BT_EMIT_TICK_EVENT(tree().id(), id());

			return detail::invoke_node_function(m_child, detail::C_TICK_FUNCTION_NAME.data()) == tick_result_ok ? tick_result_fail : tick_result_ok;
		}

	} //- bt

} //- ai