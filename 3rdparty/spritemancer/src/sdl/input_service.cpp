#include "input_service.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	cinput_service::~cinput_service()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cinput_service::on_start()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cinput_service::on_shutdown()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cinput_service::on_update(float dt)
	{
		core::cscope_mutex m(m_mutex);
		update_mouse();
		update_keyboard();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cinput_service::is_key_pressed(keyboardkey k)
	{
		return m_keyboard_state.m_current[k];
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cinput_service::is_key_released(keyboardkey k)
	{
		return !m_keyboard_state.m_current[k] && m_keyboard_state.m_previous[k];
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cinput_service::is_key_held(keyboardkey k)
	{
		return m_keyboard_state.m_current[k] && m_keyboard_state.m_previous[k];
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cinput_service::is_mouse_pressed(mousebutton b)
	{
		return m_mouse_state.m_current[b];
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cinput_service::is_mouse_released(mousebutton b)
	{
		return !m_mouse_state.m_current[b] && m_mouse_state.m_previous[b];
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cinput_service::is_mouse_held(mousebutton b)
	{
		return m_mouse_state.m_current[b] && m_mouse_state.m_previous[b];
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cinput_service::update_keyboard()
	{
		//- move state from current frame to last frame
		m_keyboard_state.m_previous.resize(m_keyboard_state.m_current.size());
		std::memmove(&m_keyboard_state.m_previous[0], &m_keyboard_state.m_current[0], m_keyboard_state.m_current.size() * sizeof(bool));

		//- populate current frame state
		int count = 0;
		const auto* state = SDL_GetKeyboardState(&count);

		m_keyboard_state.m_current.resize(count);
		std::memcpy(&m_keyboard_state.m_current[0], state, count * sizeof(uint8_t));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cinput_service::update_mouse()
	{
		//- move state from current frame to last frame
		std::memmove(&m_mouse_state.m_previous[0], &m_mouse_state.m_current[0], m_mouse_state.m_current.size() * sizeof(bool));

		//- populate current frame state
		const auto mask = SDL_GetMouseState(&m_mouse_state.m_x, &m_mouse_state.m_y);

		m_mouse_state.m_current[mousebutton_left]	= mask & SDL_BUTTON_LMASK;
		m_mouse_state.m_current[mousebutton_right]	= mask & SDL_BUTTON_RMASK;
		m_mouse_state.m_current[mousebutton_middle] = mask & SDL_BUTTON_MMASK;
		m_mouse_state.m_current[mousebutton_extra1] = mask & SDL_BUTTON_X1MASK;
		m_mouse_state.m_current[mousebutton_extra2] = mask & SDL_BUTTON_X2MASK;
	}

} //- sm