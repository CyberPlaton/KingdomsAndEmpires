#include "ecs_system.hpp"

namespace ecs
{
	RTTR_PLUGIN_REGISTRATION
	{
		using namespace rttr;

		registration::class_<csystem>("csystem")
			.constructor<flecs::world&>();

	};

	//------------------------------------------------------------------------------------------------------------------------
	csystem::csystem(flecs::world& w) :
		m_world(w)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	ecs::csystem& csystem::subsystem(subsystem_registrator_t function)
	{
		function(world());
		return *this;
	}

} //- ecs

namespace ecs::example
{
	//------------------------------------------------------------------------------------------------------------------------
	struct sbehavior_component
	{
		stringview_t m_script;
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sdestructible_component
	{
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
	struct sphysical_trigger_component
	{
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
	class csystem_example : public csystem
	{
	public:
		csystem_example(flecs::world& w) : csystem(w)
		{
			//- use constructor only to define the system
			subsystem([&](flecs::world& w)
				{
					//- create dependency graph
					auto physics_destruction_update_phase = w.entity()
						.add(flecs::Phase)
						.depends_on(flecs::OnUpdate);

					auto behavior_update_phase = w.entity()
						.add(flecs::Phase)
						.depends_on(physics_destruction_update_phase);

					//- first system of the module
					auto physics_destruction_update_system = w.system<sdestructible_component, sphysical_trigger_component>("Physics Destruction System")
						.kind(physics_destruction_update_phase)
						.each([](sdestructible_component& destr, sphysical_trigger_component& trigg)
							{
							});

					//- second system of the module
					auto behavior_update_system = w.system<const sbehavior_component>("Behavior System")
						.kind(behavior_update_phase)
						.each([](const sbehavior_component& behavior)
							{
							});

					//- Physics Destruction system will run on ecs update and Behavior system will run after it because
					//- the established dependency
				});
		};

		RTTR_ENABLE(csystem);
	};

} //- ecs::example