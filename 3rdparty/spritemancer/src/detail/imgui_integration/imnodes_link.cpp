#include "imnodes_link.hpp"

namespace imnodes
{
	//------------------------------------------------------------------------------------------------------------------------
	clink::clink(const ImNodes::LinkId& id, const ImNodes::PinId& from, const ImNodes::PinId& to) :
		m_id(id), m_start_pin(from), m_end_pin(to), m_color({1.0f, 1.0f, 1.0f, 1.0f}), m_thickness(1.0f), m_flow(false),
		m_flow_direction(ImNodes::FlowDirection::Forward)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	clink::~clink()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	clink& clink::flow(link_flow idx, const bool value)
	{
		m_flow_direction = (ImNodes::FlowDirection)idx;
		m_flow = value;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	clink& clink::thickness(const float value)
	{
		m_thickness = value;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	clink& clink::color(link_color idx, const core::scolor& value)
	{
		switch (idx)
		{
		case link_color_line:
		{
			break;
		}
		default:
		{
			m_colors.push_back((ImNodes::StyleColor)idx, value);
			break;
		}
		}
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void clink::draw()
	{
		CORE_ASSERT(m_start_pin && m_end_pin, "Invalid operation. Start and/or end pins were not set!");

		ImNodes::Link(m_id, m_start_pin, m_end_pin, m_color, m_thickness);

		if (m_flow)
		{
			ImNodes::Flow(m_id, m_flow_direction);
		}

		m_colors.clear();
		m_styles.clear();
	}

} //- imnodes