#pragma once
#include "../sm_config.hpp"
#include "../renderers/sm_renderer_bgfx.hpp"
#if CORE_PLATFORM_WINDOWS
	#define GLFW_EXPOSE_NATIVE_WIN32
#elif CORE_PLATFORM_LINUX | BX_PLATFORM_BSD
	#define GLFW_EXPOSE_NATIVE_X11
#elif CORE_PLATFORM_OSX
	#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <glfw.h>

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	class cos_glfw final : public ios
	{
	public:
		cos_glfw() = default;
		~cos_glfw() = default;
		opresult init() override final;								//- create and init of client application
		opresult shutdown() override final;							//- destroy and clean of client application
		opresult init_gfx(int w, int h,
			bool fullscreen, bool vsync) override final;			//- create graphical context

		opresult init_mainwindow(stringview_t title,
			int w, int h, bool fullscreen) override final;			//- create application main window

		opresult optional_init_event_mainloop() override final;		//- process hardware events in a loop; use where required
		opresult optional_process_event() override final;			//- process one hardware event

		void on_event(const rttr::variant& event) override final;

		core::smouse_state mouse_state() const override final { return m_mouse; }
		core::skeyboard_state keyboard_state() const override final { return m_keyboard; }
		core::sgamepad_state gamepad_state() const override final { return m_gamepad; }

		void main_window_position(int* x, int* y) override final;
		void main_window_size(int* x, int* y) override final;

		bool is_key_held(core::key k) const override final;
		bool is_key_pressed(core::key k) const override final;
		bool is_key_released(core::key k) const override final;
		bool is_modifier_active(int modifiers) const override final;

		void mouse_position(double* x, double* y) override final;
		void mouse_scroll(double* x, double* y) override final;
		bool is_mouse_button_held(core::mouse_button b) override final;
		bool is_mouse_button_pressed(core::mouse_button b) override final;
		bool is_mouse_button_released(core::mouse_button b) override final;

		float gamepad_axis(core::gamepad_axis a) override final;

	private:
		core::sgamepad_state m_gamepad;
		core::smouse_state m_mouse;
		core::skeyboard_state m_keyboard;
		GLFWwindow* m_mainwindow = nullptr;
		int m_mainwindow_width;
		int m_mainwindow_height;

		RTTR_ENABLE(ios);
	};

} //- sm