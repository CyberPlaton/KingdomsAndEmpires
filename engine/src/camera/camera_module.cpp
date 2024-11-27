#include "camera_module.hpp"
#include "../ecs/ecs_world.hpp"

namespace camera
{
	//------------------------------------------------------------------------------------------------------------------------
	void camera_sync_system(flecs::entity e, const ecs::scamera& camera)
	{
		sm::update_frame_camera(camera.m_position, camera.m_offset, camera.m_zoom, camera.m_rotation);
	}

	//------------------------------------------------------------------------------------------------------------------------
	scamera_sync_system::scamera_sync_system(ecs::cworld* w)
	{
		w->create_system(config(), &camera_sync_system);
	}

	//------------------------------------------------------------------------------------------------------------------------
	ecs::system::sconfig scamera_sync_system::config()
	{
		ecs::system::sconfig cfg;

		cfg.m_name = "scamera_sync_system";
		cfg.m_run_after = { "OnUpdate" };
		cfg.m_run_before = {};
		cfg.m_flags = ecs::system::system_flag_none;

		return cfg;
	}

	//------------------------------------------------------------------------------------------------------------------------
	scamera_module::scamera_module(ecs::cworld* w)
	{
		w->import_module<scamera_module>(config());
	}

	//------------------------------------------------------------------------------------------------------------------------
	ecs::modules::sconfig scamera_module::config()
	{
		ecs::modules::sconfig cfg;
		cfg.m_name = "scamera_module";
		cfg.m_components = { "scamera" };
		cfg.m_systems = { "scamera_sync_system" };
		cfg.m_modules = {};

		return cfg;
	}

} //- camera