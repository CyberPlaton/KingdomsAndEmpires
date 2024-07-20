#pragma once
#include <core.h>
#include <engine.h>
#include "editor_basic_node.hpp"
#include "editor_nodes_link.hpp"
#include "editor_nodes_pin.hpp"

namespace editor::nodes
{
	//- Responsible for style of current node editor.
	//------------------------------------------------------------------------------------------------------------------------
	class cstyle final
	{
	public:
		cstyle();
		~cstyle();

		ImNodesStyle& style();

	private:
		ImNodesStyle& m_style;
	};

} //- editor::nodes