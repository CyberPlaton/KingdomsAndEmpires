#include "imnodes_scopes.hpp"

namespace imnodes
{
	//- Immediate mode style link definition.
	//------------------------------------------------------------------------------------------------------------------------
	class clink final
	{
	public:
		static constexpr auto C_INVALID_ID = 0;

		enum link_flow : uint8_t
		{
			link_flow_none					= 0,
			link_flow_direction_forward		= ImNodes::FlowDirection::Forward,
			link_flow_direction_backward	= ImNodes::FlowDirection::Backward,
			link_flow_speed					= ImNodes::StyleVar_FlowSpeed,
			link_flow_duration				= ImNodes::StyleVar_FlowDuration,
			link_flow_marker_distance		= ImNodes::StyleVar_FlowMarkerDistance,
		};

		enum link_style : uint8_t
		{
			link_style_none = 0,
			link_style_strength = ImNodes::StyleVar_LinkStrength,
		};

		enum link_color : uint8_t
		{
			link_color_none = 0,
			link_color_line,
			link_color_hovered = ImNodes::StyleColor_HovLinkBorder,
			link_color_selected = ImNodes::StyleColor_SelLinkBorder,
		};

		clink(const ImNodes::LinkId& id, const ImNodes::PinId& from, const ImNodes::PinId& to);
		~clink();

		clink& flow(link_flow idx, const bool value);
		template<typename T>
		clink& flow_style(link_flow idx, const T& value);
		clink& thickness(const float value);
		clink& color(link_color idx, const core::scolor& value);
		template<typename T>
		clink& style(link_style idx, const T& value);

		void draw();

	private:
		ccolor_scope m_colors;
		cstyle_scope m_styles;
		ImVec4 m_color;
		float m_thickness;
		ImNodes::LinkId m_id;
		ImNodes::PinId m_start_pin;
		ImNodes::PinId m_end_pin;
		ImNodes::FlowDirection m_flow_direction;
		bool m_flow;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	clink& clink::flow_style(link_flow idx, const T& value)
	{
		m_styles.push_back((ImNodes::StyleVar)idx, value);
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	clink& clink::style(link_style idx, const T& value)
	{
		m_styles.push_back((ImNodes::StyleVar)idx, value);
		return *this;
	}

} //- imnodes