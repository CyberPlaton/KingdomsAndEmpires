#pragma once
#include "../plugin_module_example_common.hpp"
#include "../components/example_components.hpp"

namespace module_example
{
	void targeting_system(flecs::entity e, stargeting_component& target);
	void targeting_system_2(flecs::entity e, stargeting_component& target);
	void targeting_system_3(flecs::entity e, stargeting_component& target);
	void transform_system(flecs::entity e, stransform_component& transform);
	void replication_system(flecs::entity e, sreplicable_component& rep, const stransform_component& trans, const sidentifier_component& id);

	//- Example module system
	//------------------------------------------------------------------------------------------------------------------------
	struct EXAMPLE_API stargeting_system final
	{
		stargeting_system(ecs::cworld* w)
		{
			ecs::system::sconfig cfg;

			cfg.m_flags |= ecs::system::system_flag_multithreaded;
			cfg.m_name = "Targeting System";

			ecs::system::create_system(cfg, targeting_system);
		}
	};

	//- Duplicate of above system to be used as test in multithreaded environment
	//------------------------------------------------------------------------------------------------------------------------
	struct EXAMPLE_API stargeting_system_2 final
	{
		stargeting_system_2(flecs::world& w)
		{
			ecs::system::sconfig cfg{ w };

			cfg.m_flags |= ecs::system::system_flag_multithreaded;
			cfg.m_name = "Targeting System #2";

			ecs::system::create_system(cfg, targeting_system_2);
		}
	};

	//- Duplicate of above system to be used as test in multithreaded environment
	//------------------------------------------------------------------------------------------------------------------------
	struct EXAMPLE_API stargeting_system_3 final
	{
		stargeting_system_3(flecs::world& w)
		{
			ecs::system::sconfig cfg{ w };

			cfg.m_flags |= ecs::system::system_flag_multithreaded;
			cfg.m_name = "Targeting System #3";

			ecs::system::create_system(cfg, targeting_system_3);
		}
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct EXAMPLE_API stransform_system final
	{
		stransform_system(flecs::world& w)
		{
			ecs::system::sconfig cfg{ w };

			cfg.m_name = "Transform System";

			ecs::system::create_system(cfg, transform_system);
		}
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct EXAMPLE_API sreplication_system final
	{
		sreplication_system(flecs::world& w)
		{
			ecs::system::sconfig cfg{ w };

			cfg.m_name = "Replication System";
			cfg.m_run_after = { "Transform System" };

			ecs::system::create_system(cfg, replication_system);
		}
	};

} //- module_example