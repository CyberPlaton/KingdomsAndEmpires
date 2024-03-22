#include "window_service.hpp"

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	cwindow::cwindow(cwindow::sconfig cfg, window_t handle) :
		m_window(nullptr), m_handle(handle)
	{
		//- set render API for window

		m_window = SDL_CreateWindow(cfg.m_title.c_str(),
			cfg.m_x, cfg.m_y, cfg.m_width, cfg.m_height, cfg.m_flags);

		//- set icon if required
		if (!cfg.m_window_icon_path.empty())
		{

		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	cwindow::~cwindow()
	{
		SDL_DestroyWindow(m_window);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cwindow_service::~cwindow_service()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	ref_t<cwindow> cwindow_service::create_main_window(cwindow::sconfig cfg)
	{
		m_main_window = std::make_shared<cwindow>(cfg, invalid_handle_t);

		return m_main_window;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ref_t<cwindow> cwindow_service::main_window()
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
	ref_t<cwindow> cwindow_service::create(cwindow::sconfig cfg)
	{
		if (m_next_id < C_WINDOW_COUNT_MAX)
		{
			auto wnd = std::make_shared<cwindow>(cfg, m_next_id);

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
	ref_t<cwindow> cwindow_service::find(window_t handle)
	{
		if (m_windows.find(handle) != m_windows.end())
		{
			return m_windows.at(handle);
		}
		return nullptr;
	}

} //- engine