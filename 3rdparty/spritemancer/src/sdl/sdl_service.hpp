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

	//- more or less mirroring SDL event types
	//------------------------------------------------------------------------------------------------------------------------
	enum eventtype : uint32_t
	{
		eventtype_none = 0,
		eventtype_quit,
		eventtype_user,
		eventtype_syswm,
		eventtype_display,
		eventtype_window,
		eventtype_keyboard,
		eventtype_mouse,
		eventtype_joystick,
		eventtype_controller,
		eventtype_audiodevice,
		eventtype_touch,
		eventtype_gesture,
		eventtype_drop,
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sevent
	{
		SDL_Event m_event;
		bool m_handled = false;
	};

	//- SDL service responsible for window management, inputs and event management.
	//------------------------------------------------------------------------------------------------------------------------
	class csdl_service final : public core::cservice
	{
	public:
		static constexpr uint8_t C_EVENT_COUNT_MAX = 32;

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

		template<typename TCallable>
		void handle(TCallable&& callback);

		deque_t<sevent>& events(eventtype type);

	private:
		static sconfig s_cfg;

		umap_t<eventtype, deque_t<sevent>> m_event_map;

		RTTR_ENABLE(core::cservice);
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TCallable>
	void csdl_service::handle(eventtype type, TCallable&& callback)
	{
		auto& queue = m_event_map[type];

		for (auto& e : queue)
		{
			if (!e.m_handled && callback(e))
			{
				e.m_handled = true;
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TCallable>
	void csdl_service::handle(TCallable&& callback)
	{
		for (auto& pair : m_event_map)
		{
			for (auto& e : pair.second)
			{
				if (!e.m_handled && callback(e))
				{
					e.m_handled = true;
				}
			}
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

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(csdl_service)
	{
		rttr::registration::class_<csdl_service>("csdl_service")
			.constructor<>()
			(
				rttr::policy::ctor::as_raw_ptr
			)
			;
	}

} //- sdl