#include "imnodes_scopes.hpp"

namespace imnodes
{
	//- Immediate mode style link definition.
	//------------------------------------------------------------------------------------------------------------------------
	class clink final
	{
	public:
		clink(ImNodes::LinkId id);
		~clink();

		clink& thickness(const float value);
		clink& color(const core::scolor& value);
		clink& start(ImNodes::PinId id);
		clink& end(ImNodes::PinId id);

		void draw();

	private:
		ImVec4 m_color;
		float m_thickness;
		ImNodes::LinkId m_id;
		ImNodes::PinId m_start_pin;
		ImNodes::PinId m_end_pin;
	};

} //- imnodes