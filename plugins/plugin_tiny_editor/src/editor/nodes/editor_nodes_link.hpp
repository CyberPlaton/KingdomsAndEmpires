#pragma once
#include <core.h>
#include <engine.h>
#include "../../config.hpp"

namespace editor::nodes
{
	//- Immediate mode imnodes link defining a links visuals and settings that are used in imnodes internally
	//------------------------------------------------------------------------------------------------------------------------
	class EDITOR_API clink final
	{
	public:
		clink(int id);
		~clink();

		clink& color(ImNodesCol variable, const core::scolor& value);
		clink& style(ImNodesStyleVar variable, const vec2_t& value);
		clink& style(ImNodesStyleVar variable, float value);
		clink& thickness(float link_thickness);
		clink& icon(stringview_t value, link_icon_position where);

		void begin(int from_pin_id, int to_pin_id);
		void end();

	private:
		ImLinkData& m_data;
		bool m_enabled;
		bool m_scope;
		link_icon_position m_icon_position;

	private:
		ImLinkData& internal_data(int id);
	};

} //- editor::nodes