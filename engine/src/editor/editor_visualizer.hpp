#pragma once
#include "../ecs/ecs_system.hpp"
#include "../ecs/ecs_singleton.hpp"
#include "../ecs/ecs_component.hpp"

namespace editor
{
	//- Base visualizer class. Requires a component type as input.
	//------------------------------------------------------------------------------------------------------------------------
	template<typename... TComps>
	class cvisualizer : public ecs::csystem<TComps...>
	{
	public:
		cvisualizer(flecs::world& world, const std::string& name) :
			ecs::csystem<TComps...>(world, name)
		{
		}

	private:
	};

	//- Base visualizer class. Does not require a component as input and runs once per world tick.
	//------------------------------------------------------------------------------------------------------------------------
	class cfree_visualizer : public ecs::cfree_system
	{
	public:
		cfree_visualizer(flecs::world& world, const std::string& name) :
			ecs::cfree_system(world, name)
		{
		}

	private:
	};

} //- editor