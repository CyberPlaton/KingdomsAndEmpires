#include "imnodes_pin.hpp"

namespace imnodes
{
	//------------------------------------------------------------------------------------------------------------------------
	cpin::cpin(const ImNodes::PinId& id) :
		m_id(id)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpin::~cpin()
	{
		if (m_scope)
		{
			end();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpin& cpin::color(pin_color idx, const core::scolor& value)
	{
		CORE_ASSERT(!m_scope, "Invalid operation. Color and style variables must be set before begin of scope!");
		m_colors.push_back((ImNodes::StyleColor)idx, value);
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpin& cpin::type(pin_type value)
	{
		CORE_ASSERT(!m_scope, "Invalid operation. Color and style variables must be set before begin of scope!");
		m_type = (ImNodes::PinKind)value;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cpin::begin()
	{
		ImNodes::BeginPin(m_id, m_type);
		m_scope = true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cpin::end()
	{
		ImNodes::EndPin();
		m_styles.clear();
		m_colors.clear();
		m_scope = false;
	}

} //- imnodes