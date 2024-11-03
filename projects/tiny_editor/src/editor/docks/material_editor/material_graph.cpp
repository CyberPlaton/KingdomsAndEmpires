#include "material_graph.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	cid_generator::cid_generator(const id_t node_id_seed /*= 0*/, const id_t link_id_seed /*= 0*/) :
		m_next_node_id(node_id_seed), m_next_link_id(link_id_seed)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	id_t cid_generator::next_node_id()
	{
		return ++m_next_node_id;
	}

	//------------------------------------------------------------------------------------------------------------------------
	id_t cid_generator::next_link_id()
	{
		return ++m_next_link_id;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmaterial_graph::cmaterial_graph()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	cmaterial_graph::~cmaterial_graph()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial_graph::create_link(const id_t from_node_id,
		const id_t to_node_id, unsigned from_slot_idx, unsigned to_slot_idx)
	{
		const auto next_link_id = m_generator.next_link_id();
		auto& source_slot = node_at(from_node_id)->output_at_idx(from_slot_idx);
		auto& dest_slot = node_at(to_node_id)->input_at_idx(to_slot_idx);

		//- Create the link and store it
		slink link;
		link.m_id = next_link_id;
		link.m_from_slot = source_slot.m_id;
		link.m_from_node = from_node_id;
		link.m_to_slot = dest_slot.m_id;
		link.m_to_node = to_node_id;

		auto idx = SCAST(unsigned, m_links.size());

		m_links.push_back(link);

		m_links_lookup_table[next_link_id] = idx;

		//- Set the link data for respective node slots
		source_slot.m_link_id = next_link_id;
		dest_slot.m_link_id = next_link_id;
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
	node_ref_t cmaterial_graph::destination_node_at(const sslot& slot)
	{
		if (const auto& link = link_at(slot.m_link_id); link.m_id != C_INVALID_ID)
		{
			return node_at(link.m_to_node);
		}
		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------
	node_ref_t cmaterial_graph::source_node_at(const sslot& slot)
	{
		if (const auto& link = link_at(slot.m_link_id); link.m_id != C_INVALID_ID)
		{
			return node_at(link.m_from_node);
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