#pragma once
#include <config.hpp>
#include <core.h>

//- use this macro for defining a simple component. Simple means that
//- it does not inherit from a complex component hierarchy.
#define DECLARE_COMPONENT(c) \
static stringview_t name() { static constexpr stringview_t C_NAME = STRING(c); return C_NAME; }

namespace ecs
{
	//- base class for all components
	//------------------------------------------------------------------------------------------------------------------------
	struct ENGINE_API icomponent
	{
		static stringview_t name() { static constexpr stringview_t C_NAME = "icomponent"; return C_NAME; };
	};


} //- ecs