#include "plugin_camera_system_module.hpp"

namespace camera_system
{
	//------------------------------------------------------------------------------------------------------------------------
	bool ccamera_manager::on_start()
	{
		auto w = raylib::GetScreenWidth();
		auto h = raylib::GetScreenHeight();

		//- create engine default camera,
		//- so we can still run without a user made one
		m_default.viewport(0.0f, 0.0f, SCAST(float, w), SCAST(float, h));
		m_default.clearcolor(250, 250, 250, 250);
		m_default.move_to({ 0.0f, 0.0f });
		m_default.rotate_to(0.0f);
		m_default.zoom_to(0.5f);

		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccamera_manager::pop(stringview_t name)
	{
		auto h = algorithm::hash(name);

		if (m_active_camera == h)
		{
			logging::log_warn(fmt::format("Erasing active camera '{}'", name));
		}

		m_cameras.erase(h);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::ccamera* ccamera_manager::default_camera()
	{
		return &m_default;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::ccamera* ccamera_manager::active_camera() const
	{
		ASSERT(m_active_camera != 0, "Invalid operation. No active camera defined");

		return m_cameras.at(m_active_camera).get();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool ccamera_manager::has_active_camera() const
	{
		return m_active_camera != 0;
	}

} //- camera_system