#include "editor_nodes_context.hpp"

namespace editor::nodes
{
	//------------------------------------------------------------------------------------------------------------------------
	cstyle::cstyle() : m_style(ImNodes::GetStyle())
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cstyle::~cstyle()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	ImNodesStyle& cstyle::style()
	{
		return m_style;
	}

} //- editor::nodes