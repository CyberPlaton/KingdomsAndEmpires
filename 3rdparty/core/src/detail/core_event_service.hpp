#pragma once
#include "core_config.hpp"
#include "core_service_manager.hpp"
#include "core_mutex.hpp"

namespace events
{
	namespace window
	{
		struct sresize { int w = 0; int h = 0; };
		struct scursor { double mx = 0; double my = 0; };
		struct smouse_button { int button = 0; int action = 0; int mods = 0; };
		struct skey_button { int button = 0; int scancode = 0; int action = 0; int mods = 0; };
		struct smouse_scroll { double dx = 0.0; double dy = 0.0; };
		struct scharacter_input { unsigned codepoint = 0; };
		struct sminimize {};
		struct sunminimize {};
		struct shide {};
		struct sunhide {};
		struct sfocus {};
		struct sunfocus {};
		struct sclose {};	//- Currently only considering the main window

	} //- window

} //- events

namespace core
{
	//- Class for user defined events. Does not handle file change detection (see filewatcher), nor hardware events
	//- i.e. keyboard input, mouse input, window resizing and the like (see sdl/glfw etc. service for that).
	//-
	//- struct ExampleEvent {
	//-		float x = 0.0f, y = 0.0f;
	//-		int time = 0;
	//- };
	//-
	//- emitting an event is easy as
	//-		event_service.emit_event<ExampleEvent>();
	//-		event_service.emit_event<smy_event>(1.0f, 25.0f, 11101929);
	//-
	//- creating a listener can be done with
	//-		event_service.emplace_listener<ExampleEvent>([](const rttr::variant& var)
	//-			{
	//-				const auto& e = var.convert<ExampleEvent>();
	//-
	//-				logging::log_warn(fmt::format("Event: '{} - x={}, y={}, time={}'",
	//-					var.get_type().get_name().data(), e.x, e.y, e.time));
	//-			});
	//-
	//- Note: when handling an event and you want to hang on to that data, you can copy the event object.
	//------------------------------------------------------------------------------------------------------------------------
	class cevent_service final : public core::cservice
	{
	public:
		using event_listener_callback_t = std::function<void(const rttr::variant&)>;

		cevent_service() = default;
		~cevent_service();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float) override final;

		template<typename TEvent>
		void emit_event();

		template<typename TEvent, typename... ARGS>
		void emit_event(ARGS&&... args);

		template<typename TEvent>
		void emplace_listener(event_listener_callback_t callback);

	private:
		umap_t<rttr::type, vector_t<event_listener_callback_t>> m_listeners;
		umap_t<rttr::type, queue_t<rttr::variant>> m_queue;
		core::cmutex m_mutex;

		RTTR_ENABLE(core::cservice);
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TEvent>
	void cevent_service::emit_event()
	{
		core::cscope_mutex m(m_mutex);

		m_queue[rttr::type::get<TEvent>()].push(std::move(TEvent{}));
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TEvent, typename... ARGS>
	void cevent_service::emit_event(ARGS&&... args)
	{
		core::cscope_mutex m(m_mutex);

		m_queue[rttr::type::get<TEvent>()].push(std::move(TEvent{ args... }));
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TEvent>
	void cevent_service::emplace_listener(event_listener_callback_t callback)
	{
		core::cscope_mutex m(m_mutex);

		m_listeners[rttr::type::get<TEvent>()].emplace_back(std::move(callback));
	}

} //- core