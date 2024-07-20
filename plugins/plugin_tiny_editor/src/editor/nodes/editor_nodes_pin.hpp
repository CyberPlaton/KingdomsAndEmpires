#pragma once
#include <core.h>
#include <engine.h>

namespace editor::nodes
{
	//- Immediate mode imnodes pin defining a pins visual and settings that are used in imnodes internally
	//------------------------------------------------------------------------------------------------------------------------
	class cpin final
	{
	public:
		cpin(int id);
		~cpin();

		cpin& label(stringview_t value);
		cpin& type(pin_type value);
		cpin& shape(pin_shape value);
		cpin& color(ImNodesCol variable, const core::scolor& value);
		cpin& style(ImNodesStyleVar variable, const vec2_t& value);
		cpin& style(ImNodesStyleVar variable, float value);
		cpin& enabled(bool value);
		cpin& filled(bool value);

		void begin();
		void end();

	private:
		ImPinData& m_data;
		bool m_scope;
		bool m_filled;
		pin_shape m_shape;
		pin_type m_type;

	private:
		ImPinData& internal_data(int id);
	};

} //- editor::nodes