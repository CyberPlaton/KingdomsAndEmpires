#include "window_service.hpp"

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	cwindow_service::~cwindow_service()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	ref_t<sdl::cwindow> cwindow_service::create_main_window(sdl::cwindow::sconfig cfg)
	{
		m_main_window = std::make_shared<sdl::cwindow>(cfg, invalid_handle_t);

		return m_main_window;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ref_t<sdl::cwindow> cwindow_service::main_window()
	{
		return m_main_window;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cwindow_service::destroy_main_window()
	{
		m_main_window.reset();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cwindow_service::on_start()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cwindow_service::on_shutdown()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cwindow_service::on_update(float dt)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	ref_t<sdl::cwindow> cwindow_service::create(sdl::cwindow::sconfig cfg)
	{
		if (m_next_id < C_WINDOW_COUNT_MAX)
		{
			auto wnd = std::make_shared<sdl::cwindow>(cfg, m_next_id);

			m_windows.emplace(m_next_id++, wnd);

			return wnd;
		}
		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cwindow_service::destroy(window_t handle)
	{
		if (m_windows.find(handle) != m_windows.end())
		{
			m_windows.erase(handle);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	ref_t<sdl::cwindow> cwindow_service::find(window_t handle)
	{
		if (m_windows.find(handle) != m_windows.end())
		{
			return m_windows.at(handle);
		}
		return nullptr;
	}

} //- engine