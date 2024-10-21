#include "core_input.hpp"
#include "core_algorithm.hpp"

namespace core
{
	//------------------------------------------------------------------------------------------------------------------------
	void smouse_state::set_state(core::mouse_button mb, bool released, bool pressed, bool held)
	{
		const auto r = released ? 255 : 0;
		const auto g = pressed ? 255 : 0;
		const auto b = held ? 255 : 0;

		m_buttons[(int)mb] = ((r << 24) | (g << 16) | (b << 8) | 0);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void smouse_state::set_cursor(double x, double y)
	{
		m_x = x;
		m_y = y;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool smouse_state::is_released(core::mouse_button b) const
	{
		return algorithm::bit_check(m_buttons[(int)b], C_MOUSE_RELEASED_MASK);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool smouse_state::is_pressed(core::mouse_button b) const
	{
		return algorithm::bit_check(m_buttons[(int)b], C_MOUSE_PRESSED_MASK);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool smouse_state::is_held(core::mouse_button b) const
	{
		return algorithm::bit_check(m_buttons[(int)b], C_MOUSE_HELD_MASK);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void skeyboard_state::set_state(core::key key, bool released, bool pressed, bool held, int modifiers)
	{
		const auto r = released ? 255 : 0;
		const auto g = pressed ? 255 : 0;
		const auto b = held ? 255 : 0;

		//- Alpha channel is unused, the modifiers state is encoded separately in an integer
		m_keys[(int)key] = ((r << 24) | (g << 16) | (b << 8) | 0);
		m_modifiers = modifiers;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool skeyboard_state::is_released(core::key key) const
	{
		return algorithm::bit_check(m_keys[(int)key], C_KEY_RELEASED_MASK);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool skeyboard_state::is_pressed(core::key key) const
	{
		return algorithm::bit_check(m_keys[(int)key], C_KEY_PRESSED_MASK);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool skeyboard_state::is_held(core::key key) const
	{
		return algorithm::bit_check(m_keys[(int)key], C_KEY_HELD_MASK);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool skeyboard_state::check_modifiers(int modifiers) const
	{
		return algorithm::bit_check(m_modifiers, modifiers);
	}

} //- core