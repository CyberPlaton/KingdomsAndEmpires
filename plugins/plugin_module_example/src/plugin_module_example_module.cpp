#include "plugin_module_example_module.hpp"

namespace module_example
{
	//------------------------------------------------------------------------------------------------------------------------
	ecs::modules::sconfig smy_module::config()
	{
		ecs::modules::sconfig cfg;
		cfg.m_name = "My Example Module";
		cfg.m_components = { "stargeting_component" };
		cfg.m_systems = { "stargeting_system" };
		cfg.m_modules = {};

		return cfg;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ecs::modules::sconfig smy_second_module::config()
	{
		ecs::modules::sconfig cfg;
		cfg.m_name = "My Second Example Module";
		cfg.m_systems = { "stargeting_system", "stargeting_system_2", "stargeting_system_3" };
		cfg.m_modules = { "smy_module" }; //-- stargeting_component will be registered along with dependency module

		return cfg;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ecs::modules::sconfig smy_third_module::config()
	{
		ecs::modules::sconfig cfg;
		cfg.m_name = "My Third Example Module";
		cfg.m_components = { "stargeting_component", "stransform_component", "sidentifier_component" };
		cfg.m_systems = { "stransform_system", "sreplication_system" };

		return cfg;
	}

} //- module_example