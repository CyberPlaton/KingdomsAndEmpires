#include "imnodes_scopes.hpp"

namespace imnodes
{
	//------------------------------------------------------------------------------------------------------------------------
	class cpin final
	{
	public:
		enum pin_color : uint8_t
		{
			pin_color_none			= 0,
			pin_color_background	= ImNodes::StyleColor_PinRect,
			pin_color_border		= ImNodes::StyleColor_PinRectBorder,
		};

		enum pin_style : uint8_t
		{
			pin_style_none			= 0,
			pin_style_alignment		= ImNodes::StyleVar_PivotAlignment,
			pin_style_size			= ImNodes::StyleVar_PivotSize,
			pin_style_scale			= ImNodes::StyleVar_PivotScale,
			pin_style_corners		= ImNodes::StyleVar_PinCorners,
			pin_style_radius		= ImNodes::StyleVar_PinRadius,
			pin_style_arrow_size	= ImNodes::StyleVar_PinArrowSize,
			pin_style_arrow_width	= ImNodes::StyleVar_PinArrowWidth,
		};

		cpin(ImNodes::PinId id);
		~cpin();

		cpin& color(const core::scolor& value);

		void begin();
		void end();

	private:
		ImNodes::PinId m_id;
		ImNodes::PinKind m_type;
	};

} //- imnodes