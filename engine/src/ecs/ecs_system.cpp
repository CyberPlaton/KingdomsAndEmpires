#include "ecs_system.hpp"
#include "ecs_component.hpp"
#include "ecs_world_manager.hpp"
#include "ecs_singleton.hpp"

namespace ecs
{
} //- ecs

namespace ecs::example
{
	//------------------------------------------------------------------------------------------------------------------------
	struct sbehavior_component : public icomponent
	{
		DECLARE_COMPONENT(sbehavior_component);

		stringview_t m_script;
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sdestructible_component : public icomponent
	{
		DECLARE_COMPONENT(sdestructible_component);

		enum type
		{
			physagreggator,
			dynamic,
			duplicating,
		};

		stringview_t m_path;
		type m_type;
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sphysical_trigger_component : public icomponent
	{
		DECLARE_COMPONENT(sphysical_trigger_component);

		enum type
		{
			activating,
			deactivating,
		};

		type m_type;
		int m_value;
	};

	//- Example system. Missing is the registration in RTTR, for this checkout plugin_module_example
	//------------------------------------------------------------------------------------------------------------------------
	class csystem_example final : public csystem<sdestructible_component, sphysical_trigger_component>
	{
	public:
		csystem_example(flecs::world& w) :
			csystem<sdestructible_component, sphysical_trigger_component>
			(w, "csystem_example")
		{
			run_on(system_running_phase_on_update);

			build([&](flecs::entity e, sdestructible_component& destr, const sphysical_trigger_component& trigg)
				{
					//- with this we only define one system
				});

// 			subsystem([&](flecs::world& w) -> subsystem_registrator_return_t
// 				{
// 					//- create dependency graph
// 					auto physics_destruction_update_phase = w.entity()
// 						.add(flecs::Phase)
// 						.depends_on(flecs::OnUpdate);
// 
// 					auto behavior_update_phase = w.entity()
// 						.add(flecs::Phase)
// 						.depends_on(physics_destruction_update_phase);
// 
// 					//- first system of the module
// 					auto physics_destruction_update_system = w.system<sdestructible_component, sphysical_trigger_component>("Physics Destruction System")
// 						.kind(physics_destruction_update_phase)
// 						.each([](sdestructible_component& destr, sphysical_trigger_component& trigg)
// 							{
// 							});
// 
// 					//- second system of the module
// 					auto behavior_update_system = w.system<const sbehavior_component>("Behavior System")
// 						.kind(behavior_update_phase)
// 						.each([](const sbehavior_component& behavior)
// 							{
// 							});
// 
// 					//- Physics Destruction system will run on ecs update and Behavior system will run after it because
// 					//- the established dependency
// 					//- Return Physics as main system and Behavior as a subsystem, because it depends on Physics.
// 					return { physics_destruction_update_system, {behavior_update_system} };
// 				});
		}
	};

} //- ecs::example