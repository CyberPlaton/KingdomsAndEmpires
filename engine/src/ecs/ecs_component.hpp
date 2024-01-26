#pragma once
#include <core.h>

#define DECLARE_COMPONENT(c) \
static stringview_t name() { static constexpr stringview_t C_NAME = STRING(c); return C_NAME; }

namespace ecs
{
	//- base class for all components
	//------------------------------------------------------------------------------------------------------------------------
	struct icomponent
	{
		DECLARE_COMPONENT(icomponent);

		RTTR_ENABLE();
	};


} //- ecs