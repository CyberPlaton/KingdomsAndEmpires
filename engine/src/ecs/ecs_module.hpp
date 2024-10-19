#pragma once
#include <core.h>

namespace ecs
{
	namespace modules
	{
		//-------------------------------------------------------------------------------------------------------------------------
		struct sconfig final
		{
			string_t m_name;
			vector_t<string_t> m_components;//- Components that this module is using
			vector_t<string_t> m_systems;	//- Systems that this module is using
			vector_t<string_t> m_modules;	//- Dependent modules that must be loaded before this one
		};

	} //-- modules

} //- ecs