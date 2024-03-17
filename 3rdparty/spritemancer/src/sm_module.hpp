#pragma once
#include <core.h>
#include <sdl2.h>
#include <bx.h>
#include <bimg.h>
#include <bgfx.h>

namespace sdl
{
	//------------------------------------------------------------------------------------------------------------------------
	enum window_flag
	{
		window_flag_none = BIT(0),
		window_flag_fullscreen = BIT(1),
		window_flag_msaa = BIT(2),
		window_flag_vsync = BIT(3),
		window_flag_resizable = BIT(4),
		window_flag_decorated = BIT(5),
		window_flag_show = BIT(6),
		window_flag_focus = BIT(7),
		window_flag_minimized = BIT(8),
		window_flag_run_minimized = BIT(9),
		window_flag_has_icon = BIT(10)
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum window_resolution : uint8_t
	{
		window_resolution_custom = 0,
		window_resolution_switch,
		window_resolution_steamdeck,
		window_resolution_sxga,
		window_resolution_hd,
		window_resolution_hd_plus,
		window_resolution_fhd,
		window_resolution_wuxga,
		window_resolution_qhd,
		window_resolution_wqhd,
		window_resolution_uhd,
	};

	template<class TType>
	class isdl_resource
	{
	public:
		isdl_resource() = default;

		TType* native() const;

	private:
		TType* m_handle;
	};


	class cwindow
	{
	public:
		using window_ref_t = ref_t<SDL_Window>;

		struct sconfig
		{
			std::string m_title = {};
			std::string m_window_icon_path = {};
			window_resolution m_resolution = window_resolution_custom;
			unsigned m_x = 0;
			unsigned m_y = 0;
			unsigned m_width = 0;
			unsigned m_height = 0;
			unsigned m_target_fps = 0;
			int m_flags = 0;

			RTTR_ENABLE();
		};

		cwindow(sconfig& cfg)
		{
			m_window = std::make_shared<SDL_Window>(SDL_CreateWindow(cfg.m_title.c_str(), cfg.m_x, cfg.m_y, cfg.m_width, cfg.m_height, 0));
		}

		~cwindow()
		{

		}

		bool info(SDL_SysWMinfo* info) const
		{
			if (SDL_GetWindowWMInfo(m_window.get(), info) == SDL_TRUE)
			{
				return true;
			}
			return false;
		}

		SDL_Window* window() const
		{

		}

	private:
		window_ref_t m_window;
	};

} //- sdl