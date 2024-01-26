#pragma once
#include <flecs.h>
#include <core.h>
#include <config.hpp>
#include "ecs_system.hpp"

namespace ecs
{
	//- utility class simplifying registering a module into ecs
	//-------------------------------------------------------------------------------------------------------------------------
	class ENGINE_API imodule
	{
	public:
		imodule(flecs::world& w) : m_world(&w) {}

		template<class TModuleType>
		imodule* begin()
		{
			m_module = world().module<TModuleType>();

			return this;
		}

		template<class TDependency>
		imodule* depends_on()
		{
			world().import<TDependency>();

			return this;
		}

		template<class TSystem>
		imodule* subsystem()
		{
			TSystem s(world());

			return this;
		}

		template<class TComponent>
		imodule* comp()
		{
			world().component<TComponent>();

			return this;
		}

		bool end()
		{
			return true;
		}

		flecs::entity self_module() const;

	private:
		flecs::world* m_world;
		flecs::entity m_module;

	private:
		flecs::world& world() { ASSERT(m_world, "World for module was not set!"); return *m_world; }

		RTTR_ENABLE();
	};

} //- ecs