#pragma once
#include "../plugin_module_example_common.hpp"
#include "../components/example_components.hpp"

namespace module_example
{
	namespace
	{
		constexpr auto C_DT = 0.016f;

	} //- unnamed

	//- Example module system
	//------------------------------------------------------------------------------------------------------------------------
	class cmy_system : public ecs::csystem
	{
	public:
		cmy_system(flecs::world& w) :
			ecs::csystem(w)
		{
			subsystem([&](flecs::world& w) -> subsystem_registrator_return_t
				{
					auto sys = w.system<stargeting_component>("Targeting System")
						.kind<ecs::ssystem_phases::son_update>()
						.each([](flecs::entity e, stargeting_component& target)
							{
								//- check for first start of the system or
								//- end of cooldown and restart
								if (target.m_cooldown == stargeting_component::C_TARGET_COOLDOWN_INVALID ||
									target.m_cooldown <= 0.0f)
								{
									target.m_next_target = core::cuuid();
									target.m_cooldown = stargeting_component::C_TARGET_COOLDOWN_TIMER;

									logging::log_info(fmt::format("[Targeting System] Changing Target '{}'", target.m_next_target.string()).data());
								}
								target.m_cooldown -= C_DT;
							});

					//- Return main system, without any dependencies
					return { sys, {} };
				});
		}
	};


	//------------------------------------------------------------------------------------------------------------------------
	class sexample_module_system : public ecs::csystem
	{
	public:
		sexample_module_system(flecs::world& w) :
			ecs::csystem(w)
		{
			//- use constructor only to define the system
			subsystem([&](flecs::world& w) -> subsystem_registrator_return_t
				{
					static core::crandom rand;

					//- create dependency graph
					auto transform_update_phase = w.entity()
						.add(flecs::Phase)
						.depends_on(flecs::OnUpdate);

					auto replication_update_phase = w.entity()
						.add(flecs::Phase)
						.depends_on(transform_update_phase);

					//- first system of the module
					auto transform_update_system = w.system<stransform_component>("Transform System")
						.kind(transform_update_phase)
						.each([](flecs::entity e, stransform_component& trans)
							{
								static core::ctimer transform_timer(false);

								if (transform_timer.secs() > 5)
								{
									trans.x = rand.random_float();
									trans.y = rand.random_float();
									trans.rotation = rand.random_float();

									logging::log_debug(fmt::format("[{}][Transform] Updating transform [{}:{}:{}]",
										logging::app_runtime_ms(), trans.x, trans.y, trans.rotation));

									transform_timer.start();
								}
							});

					//- second system of the module
					auto replication_update_system = w.system<sreplicable_component, const stransform_component, const sidentifier_component>("Replication System")
						.kind(replication_update_phase)
						.each([](flecs::entity e, sreplicable_component& rep, const stransform_component& trans, const sidentifier_component& id)
							{
								static core::ctimer network_timer(false);

								//- simulate some heavy networking work
								if (network_timer.secs() > 5)
								{
									auto n = 0;
									for (auto ii = 0; ii < 25000; ++ii)
									{
										++n;
									}

									//- example: showing firstly that entity name is equal to his uuid.
									logging::log_debug(fmt::format("[{}][Network] Replicated '{}' entities. Master \"{} ()\":\n\t[{}:{}:{}]",
										logging::app_runtime_ms(), n, id.uuid.string(), e.name(), trans.x, trans.y, trans.rotation));

									network_timer.start();
								}
							});

					return { transform_update_system, {replication_update_system} };
				});
		}
	};

} //- module_example