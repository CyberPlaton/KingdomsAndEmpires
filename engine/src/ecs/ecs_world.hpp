#pragma once
#include "ecs_entity_manager.hpp"
#include "ecs_system_manager.hpp"
#include "ecs_module_manager.hpp"
#include "../physics/b2_physics.hpp"

namespace ecs
{
	//- Wrapper around flecs::world. Allows queries into current state of the ecs world and
	//- also in spatial world.
	//- Note that a world is not intended to be constructed on its own, rather through
	//- the world manager.
	//------------------------------------------------------------------------------------------------------------------------
	class cworld final : protected b2DynamicTree, core::cnon_copyable
	{
	public:
		cworld(stringview_t name);
		~cworld();

		bool QueryCallback(int proxy_id);
		float RayCastCallback(const b2RayCastInput& ray_input, int proxy_id);


		inline const flecs::world& world() const { return m_world; }
		inline flecs::world& world() { return m_world; }

		inline const centity_manager& em() const { return m_entity_manager; }
		inline centity_manager& em() { return m_entity_manager; }

		inline const csystem_manager& sm() const { return m_system_manager; }
		inline csystem_manager& sm() { return m_system_manager; }

		inline const cmodule_manager& mm() const { return m_module_manager; }
		inline cmodule_manager& mm() { return m_module_manager; }

	private:
		stringview_t m_name;
		flecs::world m_world;

		centity_manager m_entity_manager;
		csystem_manager m_system_manager;
		cmodule_manager m_module_manager;
	};

} //- ecs