#include "imnodes_scopes.hpp"

namespace imnodes
{
	//------------------------------------------------------------------------------------------------------------------------
	class cpin final
	{
	public:
		static constexpr auto C_INVALID_ID = 0;

		enum pin_type : uint8_t
		{
			pin_type_none	= 0,
			pin_type_input	= (uint8_t)ImNodes::PinKind::Input,
			pin_type_output = (uint8_t)ImNodes::PinKind::Output,
		};

		enum pin_color : uint8_t
		{
			pin_color_none			= 0,
			pin_color_background	= (uint8_t)ImNodes::StyleColor_PinRect,
			pin_color_border		= (uint8_t)ImNodes::StyleColor_PinRectBorder,
		};

		enum pin_style : uint8_t
		{
			pin_style_none			= 0,
			pin_style_alignment		= (uint8_t)ImNodes::StyleVar_PivotAlignment,
			pin_style_size			= (uint8_t)ImNodes::StyleVar_PivotSize,
			pin_style_scale			= (uint8_t)ImNodes::StyleVar_PivotScale,
			pin_style_corners		= (uint8_t)ImNodes::StyleVar_PinCorners,
			pin_style_radius		= (uint8_t)ImNodes::StyleVar_PinRadius,
			pin_style_arrow_size	= (uint8_t)ImNodes::StyleVar_PinArrowSize,
			pin_style_arrow_width	= (uint8_t)ImNodes::StyleVar_PinArrowWidth,
		};

		cpin(const ImNodes::PinId& id);
		~cpin();

		cpin& type(pin_type value);
		cpin& color(pin_color idx, const core::scolor& value);
		template<typename T>
		cpin& style(pin_style idx, const T& value);

		void begin();
		void end();

	private:
		cstyle_scope m_styles;
		ccolor_scope m_colors;
		ImNodes::PinId m_id;
		ImNodes::PinKind m_type;
		bool m_scope = false;
	};

	template<typename T>
	cpin& cpin::style(pin_style idx, const T& value)
	{
		m_styles.push_back((ImNodes::StyleVar)idx, value);
		return *this;
	}

} //- imnodes