#include "sdl_service.hpp"

namespace sdl
{
	sdl::csdl_service::sconfig csdl_service::s_cfg;

	//------------------------------------------------------------------------------------------------------------------------
	void csdl_service::configure(sconfig& cfg)
	{
		s_cfg = cfg;
	}

	//------------------------------------------------------------------------------------------------------------------------
	csdl_service::~csdl_service()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	bool csdl_service::on_start()
	{
		ASSERT(s_cfg.m_subsystems != subsystem_none, "Invalid operation. Did you configure SDL service?");

		if (SDL_Init(s_cfg.m_subsystems) == SDL_TRUE)
		{
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void csdl_service::on_shutdown()
	{
		SDL_Quit();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void csdl_service::on_update(float dt)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e) == SDL_TRUE)
		{
			const auto t = e.type;
			auto type = eventtype_none;

			//- have to convert from SDL type to internal
			switch (t)
			{
			default: break;
			case SDL_APP_TERMINATING:
			case SDL_QUIT:
			{
				type = eventtype_quit;
				break;
			}
			case SDL_DISPLAYEVENT:
			{
				type = eventtype_display;
				break;
			}
			case SDL_WINDOWEVENT:
			{
				type = eventtype_window;
				break;
			}
			case SDL_SYSWMEVENT:
			{
				type = eventtype_syswm;
				break;
			}
			case SDL_KEYUP:
			case SDL_KEYDOWN:
			{
				type = eventtype_keyboard;
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEWHEEL:
			case SDL_MOUSEMOTION:
			{
				type = eventtype_mouse;
				break;
			}
			case SDL_JOYAXISMOTION:
			case SDL_JOYBALLMOTION:
			case SDL_JOYHATMOTION:
			case SDL_JOYBUTTONDOWN:
			case SDL_JOYBUTTONUP:
			case SDL_JOYDEVICEADDED:
			case SDL_JOYBATTERYUPDATED:
			case SDL_JOYDEVICEREMOVED:
			{
				type = eventtype_joystick;
				break;
			}
			case SDL_CONTROLLERAXISMOTION:
			case SDL_CONTROLLERBUTTONDOWN:
			case SDL_CONTROLLERBUTTONUP:
			case SDL_CONTROLLERDEVICEADDED:
			case SDL_CONTROLLERDEVICEREMOVED:
			case SDL_CONTROLLERDEVICEREMAPPED:
			case SDL_CONTROLLERTOUCHPADDOWN:
			case SDL_CONTROLLERTOUCHPADMOTION:
			case SDL_CONTROLLERTOUCHPADUP:
			case SDL_CONTROLLERSENSORUPDATE:
			case SDL_CONTROLLERSTEAMHANDLEUPDATED:
			{
				type = eventtype_controller;
				break;
			}
			case SDL_FINGERDOWN:
			case SDL_FINGERUP:
			case SDL_FINGERMOTION:
			{
				type = eventtype_touch;
				break;
			}
			case SDL_MULTIGESTURE:
			case SDL_DOLLARRECORD:
			case SDL_DOLLARGESTURE:
			{
				type = eventtype_gesture;
				break;
			}
			case SDL_DROPFILE:
			case SDL_DROPTEXT:
			case SDL_DROPBEGIN:
			case SDL_DROPCOMPLETE:
			{
				type = eventtype_drop;
				break;
			}
			case SDL_AUDIODEVICEREMOVED:
			case SDL_AUDIODEVICEADDED:
			{
				type = eventtype_audiodevice;
				break;
			}
			case SDL_USEREVENT:
			{
				type = eventtype_user;
				break;
			}
			}


			auto& queue = m_event_map[type];

			while (queue.size() > C_EVENT_COUNT_MAX){queue.pop_front();}

			m_event_map[type].push_back({e});
		}
	}

	deque_t<sdl::sevent>& csdl_service::events(eventtype type)
	{
		return m_event_map.at(type);
	}

} //- sdl