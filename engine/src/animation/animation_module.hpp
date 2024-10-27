#pragma once
#include "../config.hpp"
#include "../ecs/ecs_world_context_holder.hpp"
#include "../ecs/ecs_component.hpp"
#include "../ecs/ecs_module.hpp"

namespace animation
{
	void animation_system(flecs::entity e, ecs::sanimation& animation);

	//------------------------------------------------------------------------------------------------------------------------
	struct sanimation_system final
	{
		sanimation_system() = default;
		sanimation_system(ecs::cworld* w);

		RTTR_ENABLE();
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sanimation_module final
	{
		DECLARE_MODULE(sanimation_module);

		static ecs::modules::sconfig config();

		RTTR_ENABLE();
	};

} //- animation