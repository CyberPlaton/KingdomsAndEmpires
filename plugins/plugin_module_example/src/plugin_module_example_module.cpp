#include "plugin_module_example_module.hpp"

namespace module_example
{
	//------------------------------------------------------------------------------------------------------------------------
	smy_module::smy_module(ecs::cworld* w)
	{
		ecs::modules::sconfig cfg;
		cfg.m_name = "My Example Module";
		cfg.m_components = { "stargeting_component" };
		cfg.m_systems = { "stargeting_system" };
		cfg.m_modules = {};

		w->import_module<smy_module>(cfg);
	}

	//------------------------------------------------------------------------------------------------------------------------
	smy_second_module::smy_second_module(ecs::cworld* w)
	{
		ecs::modules::sconfig cfg;
		cfg.m_name = "My Second Example Module";
		cfg.m_systems = { "stargeting_system", "stargeting_system_2", "stargeting_system_3" };
		cfg.m_modules = { "smy_module" }; //-- stargeting_component will be registered along with dependency module

		w->import_module<smy_second_module>(cfg);
	}

	//------------------------------------------------------------------------------------------------------------------------
	smy_third_module::smy_third_module(ecs::cworld* w)
	{
		ecs::modules::sconfig cfg;
		cfg.m_name = "My Third Example Module";
		cfg.m_components = { "stargeting_component", "stransform_component", "sidentifier_component" };
		cfg.m_systems = { "stransform_system", "sreplication_system" };

		w->import_module<smy_third_module>(cfg);
	}

} //- module_example