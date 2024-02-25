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
			ecs::csystem(world, name)
		{
		}

	private:
	};

} //- editor