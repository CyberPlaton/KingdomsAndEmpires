#pragma once
#include <core.h>
#include <engine.h>

namespace editor::nodes
{
	//- Helper class to instantiate a node with a special type. Does not hold any data.
	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	class cnode : public IImNode
	{
	public:
		virtual ~cnode() = default;

		cnode& group(int id);
		cnode& type(node_type value);
		cnode& label(stringview_t value, bool titlebar = true);
		cnode& color(ImNodesCol variable, const core::scolor& value);
		cnode& style(ImNodesStyleVar variable, const vec2_t& value);
		cnode& style(ImNodesStyleVar variable, float value);
	};


} //- editor::nodes