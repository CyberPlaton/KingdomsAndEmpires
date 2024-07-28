#pragma once
#include "ecs_world_context_holder.hpp"

namespace ecs
{
	//- flecs entity wrapper. Can be used in in editor or engine, however not usable in ecs systems as of now.
	//------------------------------------------------------------------------------------------------------------------------
	class centity final : public iworld_context_holder
	{
	public:
		explicit centity(flecs::entity& e, flecs::world& w);
		explicit centity(flecs::entity& e);


	private:
		flecs::entity& m_self;
	};

} //- ecs