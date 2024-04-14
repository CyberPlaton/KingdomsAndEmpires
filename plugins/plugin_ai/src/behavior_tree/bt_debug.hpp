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

	private:
		struct snode
		{
			std::string m_name;
			node_id_t m_id;

			vector_t<snode> m_children;
		};

		struct stree
		{
			snode m_root;
		};


		core::cmutex m_mutex;
	};

} //- ai::bt::debug