#pragma once
#include <core.h>
#include <sdl2.h>
#include <plugin_logging.h>

namespace sdl
{
	//------------------------------------------------------------------------------------------------------------------------
	enum subsystem : uint8_t
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
	enum eventtype : uint8_t
	{
		eventtype_none = 0,
		eventtype_common,
		eventtype_display,
		eventtype_window,
		eventtype_keyboard,
		eventtype_textedit,
		eventtype_textinput,
		eventtype_mousemotion,
		eventtype_mousebutton,
		eventtype_mousewheel,
		eventtype_joystickaxis,
		eventtype_joystickball,
		eventtype_joystickhat,
		eventtype_joystickbutton,
		eventtype_joystickdevice,
		eventtype_joystickbattery,
		eventtype_controlleraxis,
		eventtype_controllerbutton,
		eventtype_controllerdevice,
		eventtype_controllertouchpad,
		eventtype_controllersensor,
		eventtype_audio,
		eventtype_sensor,
		eventtype_quit,
		eventtype_user,
		eventtype_syswm,
		eventtype_touchfinger,
		eventtype_gesturemulti,
		eventtype_gesturedollar,
		eventtype_drop
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

	private:
		static sconfig s_cfg;

		umap_t<eventtype, vector_t<SDL_Event>> m_event_map;

		RTTR_ENABLE(core::cservice);
	};

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