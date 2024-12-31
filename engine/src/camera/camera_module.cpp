#include "camera_module.hpp"
#include "../ecs/ecs_world.hpp"

namespace camera
{
	namespace
	{
		constexpr auto C_CAMERA_ZOOM_MIN = math::C_ALMOST_EQUAL_EPSILON;

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	void camera_sync_system(flecs::entity e, const ecs::scamera& camera)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void camera_editor_controller_system(flecs::entity e, ecs::scamera& camera, const tag::seditor_camera_controller& controller)
	{
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
		cfg.m_systems = { "scamera_editor_controller_system", "scamera_sync_system" };
		cfg.m_modules = {};

		return cfg;
	}

	//------------------------------------------------------------------------------------------------------------------------
	scamera_editor_controller_system::scamera_editor_controller_system(ecs::cworld* w)
	{
		w->create_system(config(), &camera_editor_controller_system);
	}

	//------------------------------------------------------------------------------------------------------------------------
	ecs::system::sconfig scamera_editor_controller_system::config()
	{
		ecs::system::sconfig cfg;

		cfg.m_name = "scamera_editor_controller_system";
		cfg.m_run_after = {};
		cfg.m_run_before = { "scamera_sync_system" };
		cfg.m_flags = ecs::system::system_flag_immediate;

		return cfg;
	}

} //- camera