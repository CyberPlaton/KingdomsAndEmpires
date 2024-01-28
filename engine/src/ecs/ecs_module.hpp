#pragma once
#include <flecs.h>
#include <core.h>
#include <config.hpp>
#include "ecs_world_context_holder.hpp"
#include "ecs_system.hpp"

namespace ecs
{
	//- utility class simplifying registering a module into ecs
	//-------------------------------------------------------------------------------------------------------------------------
	class ENGINE_API imodule : public iworld_context_holder
	{
	public:
		imodule(ref_t<flecs::world> w);
		virtual ~imodule() = default;

		flecs::entity module() const;

		template<class TModuleType>
		imodule* begin()
		{
			m_module = world().module<TModuleType>();
			return this;
		}

		template<class TSystem>
		imodule* subsystem()
		{
			//- create and register system into current world
			TSystem sys(world_ref());
			return this;
		}

		template<class TComponent>
		imodule* comp()
		{
			world().component<TComponent>();
			return this;
		}

		bool end();

	private:
		flecs::entity m_module;
	};

} //- ecs