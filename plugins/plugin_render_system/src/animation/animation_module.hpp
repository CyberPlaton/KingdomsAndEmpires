#pragma once
#include <engine.h>
#include "../config.hpp"

namespace render_system
{
	//- TODO: we need easing functions
	//------------------------------------------------------------------------------------------------------------------------
	class RENDER_API canimation_system : public ecs::csystem<ecs::sanimation>
	{
	public:
		canimation_system(flecs::world& w) :
			ecs::csystem<ecs::sanimation>
			(w, "Animation System")
		{
			multithreaded();
			build([&](flecs::entity e, ecs::sanimation& animation)
				{
					CORE_NAMED_ZONE("Animation System");

					//- TODO: we do it like this for now. But intended was to use world().visible_entities()
					//- and update their animations only, as it does not make sense to compute so much stuff
					//- for sprites we do not even see.
				});

			run_after(flecs::OnUpdate);
		}
	};

	//------------------------------------------------------------------------------------------------------------------------
	class RENDER_API canimation_module : public ecs::imodule
	{
	public:
		canimation_module(flecs::world& w) : ecs::imodule(w)
		{
			begin<canimation_module>("Animation Module")
				.subsystem<canimation_module, canimation_system>()
				.end<canimation_module>();
		}

		RTTR_ENABLE(ecs::imodule);
	};

} //- render_system