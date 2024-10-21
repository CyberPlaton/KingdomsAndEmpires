#pragma once
#include "core_config.hpp"

namespace core
{
	//------------------------------------------------------------------------------------------------------------------------
	struct smouse_state final
	{
		static constexpr auto C_MOUSE_RELEASED_MASK = 0xff000000;
		static constexpr auto C_MOUSE_PRESSED_MASK = 0x00ff0000;
		static constexpr auto C_MOUSE_HELD_MASK = 0x0000ff00;

		smouse_state()
		{
			std::memset(&m_buttons, mouse_button_none, mouse_button_count * sizeof(int));
		}

		void set_state(core::mouse_button mb, bool released, bool pressed, bool held);
		void set_cursor(double x, double y);

		bool is_released(core::mouse_button b) const;
		bool is_pressed(core::mouse_button b) const;
		bool is_held(core::mouse_button b) const;

		double m_scroll_x = 0.0;
		double m_scroll_y = 0.0;
		double m_x = 0.0;
		double m_y = 0.0;
		int m_buttons[mouse_button_count];
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sgamepad_state final
	{
		sgamepad_state()
		{
			std::memset(&m_axis, gamepad_axis_none, gamepad_axis_count * sizeof(int));
		}

		int m_axis[gamepad_axis_count];
	};

	//- Responsible for encoding the current state of the keyboard. Each key has 3 states 'released',
	//- 'pressed' (once) and 'held' (repeat) that are updated through API and can be queried.
	//- Whether modifier keys are active can be queried by bitwise concating them from 'key_modifier' enum.
	//------------------------------------------------------------------------------------------------------------------------
	struct skeyboard_state final
	{
		static constexpr auto C_KEY_RELEASED_MASK = 0xff000000;
		static constexpr auto C_KEY_PRESSED_MASK = 0x00ff0000;
		static constexpr auto C_KEY_HELD_MASK = 0x0000ff00;

		skeyboard_state() : m_modifiers(0)
		{
			std::memset(&m_keys, key_none, key_count * sizeof(int));
		}

		void set_state(core::key key, bool released, bool pressed, bool held, int modifiers);

		bool is_released(core::key key) const;
		bool is_pressed(core::key key) const;
		bool is_held(core::key key) const;
		bool check_modifiers(int modifiers) const;

		int m_keys[key_count];
		int m_modifiers = 0;
	};

} //- core