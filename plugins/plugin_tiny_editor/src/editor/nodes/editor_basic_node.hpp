#pragma once
#include "editor_nodes_node.hpp"

namespace editor::nodes
{
	//------------------------------------------------------------------------------------------------------------------------
	class EDITOR_API cbasic_node final : public cnode<cbasic_node>
	{
		DECLARE_NODE(cbasic_node);

	public:
		~cbasic_node();

	protected:
		void do_draw(ImNodesEditorContext& ctx, const int idx) override final;
		void do_update(ImNodesEditorContext& ctx, const int idx) override final;

	private:
		cbasic_node(int id);
	};


} //- editor::nodes