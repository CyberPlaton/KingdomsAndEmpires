#include "animation_module.hpp"

namespace render_system
{
	//------------------------------------------------------------------------------------------------------------------------
	void animation_system(flecs::entity e, ecs::sanimation& animation)
	{
		CORE_NAMED_ZONE("Animation System");

		//- TODO: we do it like this for now. But intended was to use world().visible_entities()
		//- and update their animations only, as it does not make sense to compute so much stuff
		//- for sprites we do not even see.
	}

} //- render_system