#pragma once
#include "../ecs/ecs_system.hpp"
#include "../ecs/ecs_singleton.hpp"
#include "../ecs/ecs_component.hpp"

namespace editor
{
	//- Base tool class. Requires a component type as input.
	//------------------------------------------------------------------------------------------------------------------------
	template<typename... TComps>
	class ctool : public ecs::csystem<TComps...>
	{
	public:
		ctool(flecs::world& world, const std::string& name) :
			ecs::csystem<TComps...>(world, name)
		{
		}

	private:
	};

	//- Base tool class. Does not require a component as input and runs once per world tick.
	//------------------------------------------------------------------------------------------------------------------------
	class cfree_tool : public ecs::ctask
	{
	public:
		cfree_tool(flecs::world& world, const std::string& name) :
			ecs::ctask(world, name)
		{
		}

	private:
	};

} //- editor