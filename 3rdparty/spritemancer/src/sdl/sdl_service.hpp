#pragma once
#include <core.h>
#include <sdl2.h>
#include <plugin_logging.h>

namespace sdl
{
	//------------------------------------------------------------------------------------------------------------------------
	enum subsystem : uint32_t
	{
		subsystem_none		= 0,
		subsystem_timer		= SDL_INIT_TIMER,
		subsystem_audio		= SDL_INIT_AUDIO,
		subsystem_video		= SDL_INIT_VIDEO,
		subsystem_joystick	= SDL_INIT_JOYSTICK,
		subsystem_haptic	= SDL_INIT_HAPTIC,
		subsystem_controller= SDL_INIT_GAMECONTROLLER,
		subsystem_events	= SDL_INIT_EVENTS,
		subsystem_sensor	= SDL_INIT_SENSOR,
		subsystem_all		= SDL_INIT_EVERYTHING,
	};

	//- basically mirorring SDL event types
	//------------------------------------------------------------------------------------------------------------------------
	enum eventtype : uint32_t
	{
		eventtype_none = 0,
		eventtype_display = SDL_DISPLAYEVENT,
		eventtype_window = SDL_WINDOWEVENT,
		eventtype_keyup = SDL_KEYDOWN,
		eventtype_keydown = SDL_KEYUP,
		eventtype_textedit = SDL_TEXTEDITING,
		eventtype_textinput = SDL_TEXTINPUT,
		eventtype_mousemotion = SDL_MOUSEMOTION,
		eventtype_mousebuttonup = SDL_MOUSEBUTTONUP,
		eventtype_mousebuttondown = SDL_MOUSEBUTTONDOWN,
		eventtype_mousewheel = SDL_MOUSEWHEEL,
		eventtype_joystickaxismotion = SDL_JOYAXISMOTION,
		eventtype_joystickballmotion = SDL_JOYBALLMOTION,
		eventtype_joystickhatmotion = SDL_JOYHATMOTION,
		eventtype_joystickbuttonup = SDL_JOYBUTTONUP,
		eventtype_joystickbuttondown = SDL_JOYBUTTONDOWN,
		eventtype_joystickdeviceadded = SDL_JOYDEVICEADDED,
		eventtype_joystickdeviceremoved = SDL_JOYDEVICEREMOVED,
		eventtype_joystickbattery = SDL_JOYBATTERYUPDATED,
		eventtype_controlleraxismotion = SDL_CONTROLLERAXISMOTION,
		eventtype_controllerbuttonup = SDL_CONTROLLERBUTTONUP,
		eventtype_controllerbuttondown = SDL_CONTROLLERBUTTONDOWN,
		eventtype_controllerdeviceadded = SDL_CONTROLLERDEVICEADDED,
		eventtype_controllerdeviceremoved = SDL_CONTROLLERDEVICEREMOVED,
		eventtype_audiodeviceadded = SDL_AUDIODEVICEADDED,
		eventtype_audiodeviceremoved = SDL_AUDIODEVICEREMOVED,
		eventtype_quit = SDL_QUIT,
		eventtype_user = SDL_USEREVENT,
		eventtype_syswm = SDL_SYSWMEVENT,
		eventtype_touchfingerup = SDL_FINGERUP,
		eventtype_touchfingerdown = SDL_FINGERDOWN,
		eventtype_touchfingermotion = SDL_FINGERMOTION,
		eventtype_gesturemulti = SDL_MULTIGESTURE,
		eventtype_gesturedollar = SDL_DOLLARGESTURE,
		eventtype_dropfile = SDL_DROPFILE,
		eventtype_droptext = SDL_DROPTEXT,
		eventtype_dropbegin = SDL_DROPBEGIN,
		eventtype_dropcomplete = SDL_DROPCOMPLETE,
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sevent
	{
		SDL_Event m_event;
	};

	//- SDL service responsible for window management, inputs and event management.
	//------------------------------------------------------------------------------------------------------------------------
	class csdl_service final : public core::cservice
	{
	public:
		struct sconfig
		{
			int m_subsystems = subsystem_none;

			RTTR_ENABLE();
		};

		static void configure(sconfig& cfg);

		csdl_service() = default;
		~csdl_service();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float dt) override final;

		template<typename TCallable>
		void handle(eventtype type, TCallable&& callback);

	private:
		static sconfig s_cfg;

		umap_t<eventtype, queue_t<sevent>> m_event_map;

		RTTR_ENABLE(core::cservice);
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TCallable>
	void csdl_service::handle(eventtype type, TCallable&& callback)
	{
		auto& queue = m_event_map[type];
		while(!queue.empty())
		{
			const auto& e = queue.front();
			callback(e);
			queue.pop();
		}
	}

} //- sdl

namespace sdl
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(csdl_service::sconfig)
	{
		rttr::registration::class_<csdl_service::sconfig>("csdl_service::sconfig")
			.property("m_subsystems", &csdl_service::sconfig::m_subsystems)
			;

		rttr::default_constructor<csdl_service::sconfig>();
	}

} //- sdl