#pragma once
#include "../config.hpp"
#include "../ecs/ecs_world_context_holder.hpp"
#include "../ecs/ecs_component.hpp"
#include "../ecs/ecs_system.hpp"
#include "../ecs/ecs_module.hpp"

namespace camera
{
	void camera_sync_system(flecs::entity e, const ecs::scamera& camera);

	//- System responsible for syncing camera data from ecs world to spritemancer. In order for camera related systems
	//- to work as-expected, they must be defined to run before this system.
	//------------------------------------------------------------------------------------------------------------------------
	struct scamera_sync_system final
	{
		scamera_sync_system() = default;
		scamera_sync_system(ecs::cworld* w);

		static ecs::system::sconfig config();

		RTTR_ENABLE();
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct scamera_module final
	{
		scamera_module() = default;
		scamera_module(ecs::cworld* w);

		static ecs::modules::sconfig config();

		RTTR_ENABLE();
	};

} //- camera