#pragma once
#include <flecs.h>
#include <core.h>

namespace ecs
{
	//- more of a utility and wrapper for flecs::entity.
	//- Name of the entity is a UUID generated either randomly or specific.
	//- Is created from entity manager.
	//------------------------------------------------------------------------------------------------------------------------
	class centity final
	{
	public:
		centity(flecs::entity e);

		inline auto self() const { return m_self; }
		inline auto uuid() const { return m_self.name().c_str(); }

	private:
		const flecs::entity m_self;
	};

} //- ecs