#include "bt_debug.hpp"

namespace ai::bt::debug
{
	namespace
	{
		//------------------------------------------------------------------------------------------------------------------------
		static std::string insert_arrow(std::string& text, unsigned depth = 0)
		{
			if(depth > 0)
			{
				return fmt::format("{}:─^{}", "--->", text, depth);
			}
			return text:
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	cbehavior_tree_debugger::cbehavior_tree_debugger()
	{
		engine::cengine::service<engine::cevent_service>()->emplace_listener<snode_tick_event>(
			[this](const rttr::variant& var)
			{
				const auto& e = var.convert<snode_tick_event>();

				core::cscope_mutex m(m_mutex);

				//- quick emplace data for later conversion
			});

		engine::cengine::service<engine::cevent_service>()->emplace_listener<snode_attach_event>(
			[this](const rttr::variant& var)
			{
				const auto& e = var.convert<snode_attach_event>();

				core::cscope_mutex m(m_mutex);

				auto& tree = m_trees[e.m_tree];

				auto node = std::make_shared<cbehavior_tree_debugger::snode>(fmt::format("{} #{}", e.m_type.get_name().data() , e.m_node));

				tree.m_nodes[e.m_node] = node;

				if(e.m_node == 0)
				{
					m_root = node;
				}
				else
				{
					//- assumption: parent already present
					tree.m_nodes[e.m_parent].m_children.push_back(node);
				}
			});

		engine::cengine::service<engine::cevent_service>()->emplace_listener<sdecorator_attach_event>(
			[this](const rttr::variant& var)
			{
				const auto& e = var.convert<sdecorator_attach_event>();

				core::cscope_mutex m(m_mutex);

				//- quick emplace data for later conversion
			});

	}

	//------------------------------------------------------------------------------------------------------------------------
	std::string cbehavior_tree_debugger::log_tree(tree_id_t id)
	{
		const auto& tree = m_trees.at(id);

		std::string out;

		out = log_node(out, tree.m_root, 0);

		return out;
	}

	//------------------------------------------------------------------------------------------------------------------------
	std::string cbehavior_tree_debugger::log_node(std::string& out, const ref_t<snode>& node, unsigned depth/*= 0*/)
	{
		out = fmt::format("{}[{}] ", out, node.m_name,);

		for(const auto& kid : node.m_children)
		{
			log_node(out, kid, depth + 1);
		}
	}

} //- namespace ai::bt::debug