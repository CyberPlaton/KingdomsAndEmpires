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
		//- move state from current frame to last frame
		m_previous.resize(m_current.size());
		std::memmove(&m_previous[0], &m_current[0], m_current.size() * sizeof(uint8_t));

		//- populate current frame state
		int count = 0;
		const auto* state = SDL_GetKeyboardState(&count);

		m_current.resize(count);
		std::memcpy(&m_current[0], state, count * sizeof(uint8_t));
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cinput_service::is_key_pressed(keyboardkey k)
	{
		return m_current[k];
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cinput_service::is_key_released(keyboardkey k)
	{
		return !m_current[k] && m_previous[k];
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cinput_service::is_key_held(keyboardkey k)
	{
		return m_current[k] && m_previous[k];
	}

} //- sm