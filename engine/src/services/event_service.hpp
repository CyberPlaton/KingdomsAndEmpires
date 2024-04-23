#pragma once
#include <core.h>

namespace engine
{
	//- Class for user defined events. Does not handle file change detection (see filewatcher), nor hardware events
	//- i.e. keyboard input, mouse input, window resizing and the like (see sdl service for that).
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

} //- engine

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(cevent_service)
	{
		rttr::registration::class_<cevent_service>("cevent_service")
			.constructor<>()
			(
				rttr::policy::ctor::as_raw_ptr
			)
			;
	}

} //- engine