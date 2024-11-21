#include "animation_module.hpp"
#include "../ecs/ecs_world.hpp"

namespace animation
{
	//------------------------------------------------------------------------------------------------------------------------
	void animation_system(ecs::centity e, ecs::sanimation& animation)
	{
		CORE_NAMED_ZONE("Animation System");

		//- TODO: we do it like this for now. But intended was to use world().visible_entities()
		//- and update their animations only, as it does not make sense to compute so much stuff
		//- for sprites we do not even see.
	}

	//------------------------------------------------------------------------------------------------------------------------
	sanimation_module::sanimation_module(ecs::cworld* w)
	{
		w->import_module<sanimation_module>(config());
	}

	//------------------------------------------------------------------------------------------------------------------------
	ecs::modules::sconfig sanimation_module::config()
	{
		ecs::modules::sconfig cfg;
		cfg.m_name = "sanimation_module";
		cfg.m_components = { "sanimation" };
		cfg.m_systems = { "sanimation_system" };
		cfg.m_modules = { "srender_module" };

		return cfg;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sanimation_system::sanimation_system(ecs::cworld* w)
	{
		w->create_system(config(), &animation_system);
	}

	//------------------------------------------------------------------------------------------------------------------------
	ecs::system::sconfig sanimation_system::config()
	{
		ecs::system::sconfig cfg;
		cfg.m_name = "sanimation_system";
		cfg.m_run_after = { "OnUpdate" };
		cfg.m_run_before = { "srender_frame_begin_system" };

		return cfg;
	}

} //- animation