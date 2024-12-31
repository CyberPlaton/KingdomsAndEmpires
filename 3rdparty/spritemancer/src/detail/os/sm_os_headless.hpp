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
		opresult init_gfx(int w, int h, bool fullscreen, bool vsync) override final { return opresult_ok; }
		opresult init_mainwindow(stringview_t title, int w, int h, bool fullscreen) override final { return opresult_ok; }
		opresult post_init() override final { return opresult_ok; }
		opresult process_events() override final { return opresult_ok; }
		void main_window_position(int* x, int* y) override final {}
		void main_window_size(int* x, int* y) override final {}
		core::smouse_state mouse_state() const override final { return {}; }
		core::skeyboard_state keyboard_state() const override final { return {}; }
		core::sgamepad_state gamepad_state() const override final { return {}; }
		float frametime() const override final { return 0.016f; }
		unsigned read_input_character() override final { return (unsigned)'\0'; }
		void mouse_position(double* x, double* y) override final {}
		void mouse_scroll_dt(double* x, double* y) override final {}
		bool is_key_held(core::key k) const override final { return false; }
		bool is_key_pressed(core::key k) const override final { return false; }
		bool is_key_released(core::key k) const override final { return false; }
		bool is_modifier_active(int modifiers) const override final { return false; }
		bool is_mouse_button_held(core::mouse_button b) override final { return false; }
		bool is_mouse_button_pressed(core::mouse_button b) override final { return false; }
		bool is_mouse_button_released(core::mouse_button b) override final { return false; }
		float gamepad_axis(core::gamepad_axis a) override final { return 0.016f; }

		RTTR_ENABLE(ios);
	};

} //- sm