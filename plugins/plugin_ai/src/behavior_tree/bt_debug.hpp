#pragma once
#include "../detail/ai_common.hpp"

namespace ai::bt::debug
{
	//- Central class for gathering and organizing behavior Tree information.
	//- Note: requires engine and services to be initialized at construction.
	//-
	//- TODO:
	//- is the data in 'stick_event' enough to construct reasonably informative debug information?
	//------------------------------------------------------------------------------------------------------------------------
	class cbehavior_tree_debugger
	{
	public:
		STATIC_INSTANCE_EX(cbehavior_tree_debugger);

		cbehavior_tree_debugger();
		~cbehavior_tree_debugger();

		std::string log_tree(tree_id_t id);
		std::string log_node(std::string& out, const ref_t<snode>& node, unsigned depth = 0);

	private:
		struct snode
		{
			std::string m_name;
			vector_t<ref_t<snode>> m_children;
		};

		struct stree
		{
			//- recursive representation used for visualization
			ref_t<snode> m_root;
			umap_t<node_id_t, ref_t<snode>> m_nodes;
		};

		umap_t<tree_id_t, stree> m_trees;
		core::cmutex m_mutex;

	private:

	};

} //- ai::bt::debug