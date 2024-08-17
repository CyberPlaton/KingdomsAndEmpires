#include "material_graph.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	cmaterial_graph::cmaterial_graph()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	cmaterial_graph::~cmaterial_graph()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial_graph::create_link(const id_t id, const id_t from_node_id,
		const id_t to_node_id, const id_t from_slot_id, const id_t to_slot_id)
	{
		//- Create the link and store it
		slink link;
		link.m_id = id;
		link.m_from_slot = from_slot_id;
		link.m_from_node = from_node_id;
		link.m_to_slot = to_slot_id;
		link.m_to_node = to_node_id;

		auto idx = SCAST(unsigned, m_links.size());

		m_links.push_back(link);

		m_links_lookup_table[id] = idx;

		//- Set the link data for respective node slots
		node_at(from_node_id)->output_at(from_slot_id).m_link_id = id;
		node_at(to_node_id)->input_at(to_slot_id).m_link_id = id;
	}

	//------------------------------------------------------------------------------------------------------------------------
	node_ref_t cmaterial_graph::node_at(id_t id)
	{
		if (const auto it = m_nodes_lookup_table.find(id); it != m_nodes_lookup_table.end())
		{
			return m_nodes[it->second];
		}
		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------
	slink cmaterial_graph::link_at(id_t id)
	{
		if (const auto it = m_links_lookup_table.find(id); it != m_links_lookup_table.end())
		{
			return m_links[it->second];
		}
		return {};
	}

} //- editor