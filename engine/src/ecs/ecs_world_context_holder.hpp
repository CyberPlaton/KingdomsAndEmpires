#pragma once
#include <config.hpp>
#include <core.h>
#include <flecs.h>

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	class iworld_context_holder
	{
	public:
		iworld_context_holder() = default;
		~iworld_context_holder() = default;

	protected:
		flecs::world& world();
	};

} //- ecs