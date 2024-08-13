#include "imnodes_link.hpp"

namespace imnodes
{
	//------------------------------------------------------------------------------------------------------------------------
	clink::clink(ImNodes::LinkId id) :
		m_id(id), m_color({1.0f, 1.0f, 1.0f, 1.0f}), m_thickness(1.0f)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	clink::~clink()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	clink& clink::thickness(const float value)
	{
		m_thickness = value;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	clink& clink::color(const core::scolor& value)
	{
		const auto v = value.normalize();
		m_color = { v.r, v.g, v.b, v.a };
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	clink& clink::start(ImNodes::PinId id)
	{
		m_start_pin = id;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	clink& clink::end(ImNodes::PinId id)
	{
		m_end_pin = id;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void clink::draw()
	{
		CORE_ASSERT(m_start_pin && m_end_pin, "Invalid operation. Start and/or end pins were not set!");

		ImNodes::Link(m_id, m_start_pin, m_end_pin, m_color, m_thickness);
	}

} //- imnodes