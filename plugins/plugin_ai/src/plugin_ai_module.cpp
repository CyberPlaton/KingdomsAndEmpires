#include "plugin_ai_module.hpp"

namespace ai
{
	namespace bt
	{

		tick_result ibehavior_tree_context_holder::do_tick_child(uint32_t i)
		{
			return tree().tick_node(i);
		}

	} //- bt

} //- ai