#pragma once
#include <flecs.h>
#include <core.h>

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	class imodule
	{
	public:
		imodule(flecs::world& world) {};
		virtual ~imodule() = default;
	};

} //- ecs