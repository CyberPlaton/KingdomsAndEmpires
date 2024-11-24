#include "example_system.hpp"

namespace module_example
{
	namespace
	{
		constexpr auto C_DT = 0.016f;

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	void targeting_system(flecs::entity e, stargeting_component& target)
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
	}

	//------------------------------------------------------------------------------------------------------------------------
	void targeting_system_2(flecs::entity e, stargeting_component& target)
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
	}

	//------------------------------------------------------------------------------------------------------------------------
	void targeting_system_3(flecs::entity e, stargeting_component& target)
	{
		CORE_NAMED_ZONE("Targeting System #3");

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

		target.m_cooldown -= C_DT * rand.in_range_float(0.0f, 2.75f);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void transform_system(flecs::entity e, stransform_component& transform)
	{
		static core::ctimer transform_timer(false);

		CORE_NAMED_ZONE("Transform System");

		if (transform_timer.secs() > 5)
		{
			core::crandom rand;

			transform.x = rand.in_range_float(0.0f, 10.0f);
			transform.y = rand.in_range_float(0.0f, 10.0f);
			transform.rotation = rand.in_range_float(0.0f, 365.0f);

			log_debug(fmt::format("[{}][Transform] Updating transform [{}:{}:{}]",
				logging::clog::instance().runtime_ms(), transform.x, transform.y, transform.rotation));

			transform_timer.start();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void replication_system(flecs::entity e, sreplicable_component& rep, const stransform_component& trans, const sidentifier_component& id)
	{
		static core::ctimer network_timer(false);

		CORE_NAMED_ZONE("Replication System");

		//- simulate some heavy networking work
		if (network_timer.secs() > 5)
		{
			auto n = 0;
			for (auto ii = 0; ii < 25000; ++ii)
			{
				++n;
			}

			//- example: showing firstly that entity name is equal to his uuid.
			log_debug(fmt::format("[{}][Network] Replicated '{}' entities. Master \"{} ({})\":\n\t[{}:{}:{}]",
				logging::clog::instance().runtime_ms(), n, id.uuid.string(), e.name(), trans.x, trans.y, trans.rotation));

			network_timer.start();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	stargeting_system::stargeting_system(ecs::cworld* w)
	{
		w->create_system(config(), targeting_system);
	}

	//------------------------------------------------------------------------------------------------------------------------
	ecs::system::sconfig stargeting_system::config()
	{
		ecs::system::sconfig cfg;

		cfg.m_flags |= ecs::system::system_flag_multithreaded;
		cfg.m_name = "stargeting_system";

		return cfg;
	}

	//------------------------------------------------------------------------------------------------------------------------
	stargeting_system_2::stargeting_system_2(ecs::cworld* w)
	{
		w->create_system(config(), targeting_system_2);
	}

	//------------------------------------------------------------------------------------------------------------------------
	ecs::system::sconfig stargeting_system_2::config()
	{
		ecs::system::sconfig cfg;

		cfg.m_flags |= ecs::system::system_flag_multithreaded;
		cfg.m_name = "stargeting_system_2";

		return cfg;
	}

	//------------------------------------------------------------------------------------------------------------------------
	stargeting_system_3::stargeting_system_3(ecs::cworld* w)
	{
		w->create_system(config(), targeting_system_3);
	}

	//------------------------------------------------------------------------------------------------------------------------
	ecs::system::sconfig stargeting_system_3::config()
	{
		ecs::system::sconfig cfg;

		cfg.m_flags |= ecs::system::system_flag_multithreaded;
		cfg.m_name = "stargeting_system_3";

		return cfg;
	}

	//------------------------------------------------------------------------------------------------------------------------
	stransform_system::stransform_system(ecs::cworld* w)
	{
		w->create_system(config(), transform_system);
	}

	//------------------------------------------------------------------------------------------------------------------------
	ecs::system::sconfig stransform_system::config()
	{
		ecs::system::sconfig cfg;

		cfg.m_name = "stransform_system";
		cfg.m_run_after = { "OnUpdate" };

		return cfg;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ecs::system::sconfig sreplication_system::config()
	{
		ecs::system::sconfig cfg;

		cfg.m_name = "sreplication_system";
		cfg.m_run_after = { "stransform_system" };

		return cfg;
	}

} //- module_example