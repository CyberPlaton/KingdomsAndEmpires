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
		sm::update_frame_camera(camera.m_position, camera.m_offset, camera.m_zoom, camera.m_rotation);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void camera_editor_controller_system(flecs::entity e, ecs::scamera& camera, const tag::seditor_camera_controller& controller)
	{
		//- FIXME: we require an input service, for now we use raylib directly.

		auto translation_speed_modifier = 1.0f;
		auto zoom_speed_modifier = 0.01f;
		auto translation_speed = 50.0f * raylib::GetFrameTime();

		if (!math::inbetween(camera.m_zoom, C_CAMERA_ZOOM_MIN, 1.0f))
		{
			translation_speed = (-translation_speed);
		}

		if (raylib::IsKeyDown(raylib::KEY_LEFT_SHIFT))
		{
			translation_speed_modifier = 150.0f;
			zoom_speed_modifier = 0.1f;
		}

		if (raylib::IsKeyDown(raylib::KEY_A))
		{
			camera.m_position.x += translation_speed * translation_speed_modifier;
		}
		if (raylib::IsKeyDown(raylib::KEY_D))
		{
			camera.m_position.x -= translation_speed * translation_speed_modifier;
		}
		if (raylib::IsKeyDown(raylib::KEY_W))
		{
			camera.m_position.y += translation_speed * translation_speed_modifier;
		}
		if (raylib::IsKeyDown(raylib::KEY_S))
		{
			camera.m_position.y -= translation_speed * translation_speed_modifier;
		}

		if (raylib::IsMouseButtonDown(raylib::MOUSE_BUTTON_MIDDLE))
		{
			auto mouse_delta = raylib::GetMouseDelta();

			if (math::inbetween(camera.m_zoom, C_CAMERA_ZOOM_MIN, 1.0f))
			{
				auto inverse_zoom = 1.0f / camera.m_zoom;
				camera.m_position.x += mouse_delta.x * inverse_zoom;
				camera.m_position.y += mouse_delta.y * inverse_zoom;
			}
			else
			{
				camera.m_position.x += mouse_delta.x * (-camera.m_zoom);
				camera.m_position.y += mouse_delta.y * (-camera.m_zoom);
			}
		}

		if (const auto wheel_delta = raylib::GetMouseWheelMove(); wheel_delta != 0.0f)
		{
			camera.m_zoom += wheel_delta * zoom_speed_modifier;

			camera.m_zoom = glm::max(camera.m_zoom, C_CAMERA_ZOOM_MIN);
		}
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