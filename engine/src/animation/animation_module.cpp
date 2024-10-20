#include "animation_module.hpp"
#include "../ecs/ecs_world.hpp"

namespace animation
{
	//------------------------------------------------------------------------------------------------------------------------
	void animation_system(flecs::entity e, ecs::sanimation& animation)
	{
		CORE_NAMED_ZONE("Animation System");

		//- TODO: we do it like this for now. But intended was to use world().visible_entities()
		//- and update their animations only, as it does not make sense to compute so much stuff
		//- for sprites we do not even see.
	}

	//------------------------------------------------------------------------------------------------------------------------
	sanimation_module::sanimation_module(ecs::cworld* w)
	{
		ecs::modules::sconfig cfg;
		cfg.m_name = "Animation Module";
		cfg.m_components = { "sanimation" };
		cfg.m_systems = { "sanimation_system" };
		cfg.m_modules = {};

		w->import_module<sanimation_module>(cfg);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sanimation_system::sanimation_system(ecs::cworld* w)
	{
		ecs::system::sconfig cfg;
		cfg.m_name = "Animation System";

		w->create_system(cfg, &animation_system);
	}

} //- animation