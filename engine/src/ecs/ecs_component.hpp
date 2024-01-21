#pragma once
#include <core.h>

namespace ecs
{
#define DECLARE_COMPONENT(c) \
static stringview_t name() { static constexpr stringview_t C_NAME = STRING(c); return C_NAME; }


	//- base class for all components
	//------------------------------------------------------------------------------------------------------------------------
	struct icomponent
	{
		DECLARE_COMPONENT(icomponent);

		RTTR_ENABLE();
	};


} //- ecs