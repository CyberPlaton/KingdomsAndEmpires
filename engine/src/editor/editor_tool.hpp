#pragma once
#include "../ecs/ecs_system.hpp"
#include "../ecs/ecs_singleton.hpp"
#include "../ecs/ecs_component.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	template<typename... TComps>
	class ctool : public ecs::csystem<TComps...>
	{
	public:
		ctool(flecs::world& world, const std::string& name) :
			ecs::csystem(world, name)
		{
		}

	private:
	};

} //- editor