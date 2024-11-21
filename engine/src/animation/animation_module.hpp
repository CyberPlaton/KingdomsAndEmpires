#pragma once
#include "../config.hpp"
#include "../ecs/ecs_world_context_holder.hpp"
#include "../ecs/ecs_component.hpp"
#include "../ecs/ecs_system.hpp"
#include "../ecs/ecs_module.hpp"

namespace animation
{
	void animation_system(ecs::centity e, ecs::sanimation& animation);

	//------------------------------------------------------------------------------------------------------------------------
	struct sanimation_system final
	{
		sanimation_system() = default;
		sanimation_system(ecs::cworld* w);

		static ecs::system::sconfig config();

		RTTR_ENABLE();
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sanimation_module final
	{
		sanimation_module() = default;
		sanimation_module(ecs::cworld* w);

		static ecs::modules::sconfig config();

		RTTR_ENABLE();
	};

} //- animation