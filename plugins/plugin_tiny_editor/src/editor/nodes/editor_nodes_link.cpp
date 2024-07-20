#include "editor_nodes_link.hpp"

namespace editor::nodes
{
	//------------------------------------------------------------------------------------------------------------------------
	clink::clink(int id) : m_data(internal_data(id)), m_enabled(true), m_scope(false), m_icon_position(link_icon_position_none)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	clink::~clink()
	{
		end();
	}

	//------------------------------------------------------------------------------------------------------------------------
	clink& clink::thickness(float link_thickness)
	{
		m_data.Thickness = link_thickness;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	clink& clink::icon(stringview_t value, link_icon_position where)
	{
		m_data.Label = value.data();
		m_data.LabelPosition = (ImNodesLinkLabelPosition)where;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void clink::begin(int from_pin_id, int to_pin_id)
	{
		ImNodes::Link(m_data.Id, from_pin_id, to_pin_id);
		m_scope = true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void clink::end()
	{
		m_scope = false;

		//- reset colors and style variables
	}

	//------------------------------------------------------------------------------------------------------------------------
	ImLinkData& clink::internal_data(int id)
	{
		auto& context = ImNodes::EditorContextGet();
		const int node_idx = ImNodes::ObjectPoolFindOrCreateIndex(context.Links, id);
		return context.Links.Pool[node_idx];
	}

	//------------------------------------------------------------------------------------------------------------------------
	clink& clink::color(ImNodesCol variable, const core::scolor& value)
	{
		CORE_ASSERT(variable >= ImNodesCol_Link && variable <= ImNodesCol_LinkSelected,
			"Invalid operation. Incorrect color variable selected!");

		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	clink& clink::style(ImNodesStyleVar variable, const vec2_t& value)
	{
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	clink& clink::style(ImNodesStyleVar variable, float value)
	{
		return *this;
	}

} //- editor::nodes