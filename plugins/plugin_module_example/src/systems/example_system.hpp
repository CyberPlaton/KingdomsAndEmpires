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
	class cmy_system : public ecs::csystem<stargeting_component>
	{
	public:
		cmy_system(flecs::world& w) :
			ecs::csystem<stargeting_component>
			(w, "Targeting System")
		{
			multithreaded();
			build([&](flecs::entity e, stargeting_component& target)
				{
					CORE_NAMED_ZONE("Targeting System");

					//- check for first start of the system or
					//- end of cooldown and restart
					if (target.m_cooldown == stargeting_component::C_TARGET_COOLDOWN_INVALID ||
						target.m_cooldown <= 0.0f)
					{
						target.m_next_target = core::cuuid();
						target.m_cooldown = stargeting_component::C_TARGET_COOLDOWN_TIMER;

						//logging::log_info(fmt::format("[Targeting System] Changing Target '{}' for '{}'",
						//	target.m_next_target.string(), e.name().c_str()));
					}
					target.m_cooldown -= C_DT;
				});

			run_after(flecs::OnUpdate);
		}
	};

	//- mainly a duplicate of above system to be used as test in multithreaded environment
	//------------------------------------------------------------------------------------------------------------------------
	class cmy_second_system : public ecs::csystem<stargeting_component>
	{
	public:
		cmy_second_system(flecs::world& w) :
			ecs::csystem<stargeting_component>
			(w, "Targeting System #2")
		{
			multithreaded();
			build([&](flecs::entity e, stargeting_component& target)
				{
					CORE_NAMED_ZONE("Targeting System #2");

					//- check for first start of the system or
					//- end of cooldown and restart
					if (target.m_cooldown == stargeting_component::C_TARGET_COOLDOWN_INVALID ||
						target.m_cooldown <= 0.0f);
					{
						target.m_next_target = core::cuuid();
						target.m_cooldown = stargeting_component::C_TARGET_COOLDOWN_TIMER;

						//logging::log_warn(fmt::format("[Targeting System #2] Changing Target '{}' for '{}'",
						//	target.m_next_target.string(), e.name().c_str()));
					}

					core::crandom rand;

					target.m_cooldown -= C_DT * rand.in_range_float(0.0f, 1.75f);
				});

			run_after(flecs::OnUpdate);
		}
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cmy_third_system : public ecs::csystem<stargeting_component>
	{
	public:
		cmy_third_system(flecs::world& w) :
			ecs::csystem<stargeting_component>
			(w, "Targeting System #3")
		{
			multithreaded();
			build([&](flecs::entity e, stargeting_component& target)
				{
					CORE_NAMED_ZONE("Targeting System #3");

					//- check for first start of the system or
					//- end of cooldown and restart
					if (target.m_cooldown == stargeting_component::C_TARGET_COOLDOWN_INVALID ||
						target.m_cooldown <= 0.0f);
					{
						target.m_next_target = core::cuuid();
						target.m_cooldown = stargeting_component::C_TARGET_COOLDOWN_TIMER;

						//logging::log_debug(fmt::format("[Targeting System #3] Changing Target '{}' for '{}'",
						//	target.m_next_target.string(), e.name().c_str()));
					}

					core::crandom rand;

					target.m_cooldown -= C_DT * rand.in_range_float(0.0f, 1.75f);
				});

			run_after(flecs::OnUpdate);
		}
	};

	//- 
	//------------------------------------------------------------------------------------------------------------------------
	class cexample_transform_system : public ecs::csystem<stransform_component>
	{
	public:
		cexample_transform_system(flecs::world& world) :
			ecs::csystem<stransform_component>(world, "Transform System")
		{
			auto update_phase = world.entity("Transform Update Phase")
				.add(flecs::Phase)
				.depends_on(flecs::OnUpdate);

			build([&](flecs::entity e, stransform_component& tr)
				{
					static core::ctimer transform_timer(false);

					if (transform_timer.secs() > 5)
					{
						core::crandom rand;

						tr.x = rand.in_range_float(0.0f, 10.0f);
						tr.y = rand.in_range_float(0.0f, 10.0f);
						tr.rotation = rand.in_range_float(0.0f, 365.0f);

						logging::log_debug(fmt::format("[{}][Transform] Updating transform [{}:{}:{}]",
							logging::app_runtime_ms(), tr.x, tr.y, tr.rotation));

						transform_timer.start();
					}
				});

			run_after(update_phase);
		}
	};

	//------------------------------------------------------------------------------------------------------------------------
	class sexample_module_system : public ecs::csystem<sreplicable_component, stransform_component, sidentifier_component>
	{
	public:
		sexample_module_system(flecs::world& w) :
			ecs::csystem<sreplicable_component, stransform_component, sidentifier_component>
			(w, "Replication System")
		{
			auto update_phase = w.entity("Replication Update Phase")
				.add(flecs::Phase)
				.depends_on(w.lookup("Transform Update Phase"));

			build([&](flecs::entity e, sreplicable_component& rep, const stransform_component& trans,
				const sidentifier_component& id)
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
						logging::log_debug(fmt::format("[{}][Network] Replicated '{}' entities. Master \"{} ({})\":\n\t[{}:{}:{}]",
							logging::app_runtime_ms(), n, id.uuid.string(), e.name(), trans.x, trans.y, trans.rotation));

						network_timer.start();
					}
				});

			//- Note: another possibility should be to write:
			//- kind("Transform Update Phase");
			//- without the need to create a new phase as above.
			run_after("Replication Update Phase");
		}
	};

} //- module_example