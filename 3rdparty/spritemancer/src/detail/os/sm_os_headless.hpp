#pragma once
#include "../sm_config.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	class cos_headless final : public ios
	{
	public:
		cos_headless() = default;
		~cos_headless() = default;
		opresult init() override final { return opresult_ok; }
		opresult shutdown() override final { return opresult_ok; }
		opresult init_gfx(int w, int h,
			bool fullscreen, bool vsync) override final { return opresult_ok; }

		opresult init_mainwindow(stringview_t title,
			int w, int h, bool fullscreen) override final { return opresult_ok; }

		opresult optional_init_event_mainloop() override final { return opresult_ok; }
		opresult optional_process_event() override final { return opresult_ok; }

		void main_window_position(int* x, int* y) override final {}
		void main_window_size(int* x, int* y) override final {}

		core::smouse_state mouse_state() const override final {}
		core::skeyboard_state keyboard_state() const override final {}
		core::sgamepad_state gamepad_state() const override final {}

		void on_window_resize_event(int width, int height) override final {}
		void on_key_event(int key, int scancode, int action, int mods) override final {}
		void on_mouse_button_event(int button, int action, int mods) override final {}
		void on_cursor_event(double mx, double my) override final {}

		void mouse_position(double* x, double* y) {}
		bool is_key_held(core::key k) override final { return false; }
		bool is_key_pressed(core::key k) override final { return false; }
		bool is_key_released(core::key k) override final { return false; }

		bool is_mouse_button_held(core::mouse_button b) override final { return false; }
		bool is_mouse_button_pressed(core::mouse_button b) override final { return false; }
		bool is_mouse_button_released(core::mouse_button b) override final { return false; }

		float gamepad_axis(core::gamepad_axis a) override final { return 0.0f; }

		RTTR_ENABLE(ios);
	};

} //- sm