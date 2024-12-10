#pragma once
#include "../sm_config.hpp"
#include "../renderers/sm_renderer_bgfx.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	class cos_glfw final : public ios
	{
	public:
		cos_glfw();
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

		unsigned read_input_character() override final;

		void main_window_position(int* x, int* y) override final;
		void main_window_size(int* x, int* y) override final;

		bool is_key_held(core::key k) const override final;
		bool is_key_pressed(core::key k) const override final;
		bool is_key_released(core::key k) const override final;
		bool is_modifier_active(int modifiers) const override final;

		void mouse_position(double* x, double* y) override final;
		void mouse_scroll_dt(double* x, double* y) override final;
		bool is_mouse_button_held(core::mouse_button b) override final;
		bool is_mouse_button_pressed(core::mouse_button b) override final;
		bool is_mouse_button_released(core::mouse_button b) override final;

		float gamepad_axis(core::gamepad_axis a) override final;

	private:
		core::skeyboard_state m_keyboard;
		char m_input_chars[256] = { '\0' };
		core::smouse_state m_mouse;
		core::sgamepad_state m_gamepad;
		GLFWwindow* m_mainwindow = nullptr;
		int m_mainwindow_width;
		int m_mainwindow_height;
		double m_scroll_dt_x = 0.0;
		double m_scroll_dt_y = 0.0;
		double m_previous_mouse_scroll_x = 0.0;
		double m_previous_mouse_scroll_y = 0.0;

		RTTR_ENABLE(ios);
	};

} //- sm