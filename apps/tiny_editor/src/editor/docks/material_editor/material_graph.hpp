#pragma once
#include "material_common.hpp"

namespace editor
{
	//- Responsible for generating ids of nodes and links. The pins are identified at runtime through their index.
	//- Each material graph has its own generator responsible and valid only for that one instance.
	//------------------------------------------------------------------------------------------------------------------------
	class cid_generator final
	{
	public:
		cid_generator(const id_t node_id_seed = 0, const id_t link_id_seed = 0);
		~cid_generator() = default;

		id_t next_node_id();
		id_t next_link_id();

	private:
		id_t m_next_node_id;
		id_t m_next_link_id;
	};

	//- Class containing editable representation of a shader in form of nodes, links and data that is used
	//- to generate valid and functioning shader code understandable by the runtime engine.
	//------------------------------------------------------------------------------------------------------------------------
	class cmaterial_graph final
	{
	public:
		cmaterial_graph();
		~cmaterial_graph();

		template<typename T, typename... ARGS>
		node_ref_t create_node(ARGS&&... args);
		void create_link(const id_t from_node_id, const id_t to_node_id, unsigned from_slot_idx, unsigned to_slot_idx);
		node_ref_t node_at(id_t id);
		slink link_at(id_t id);
		node_ref_t destination_node_at(const sslot& slot);
		node_ref_t source_node_at(const sslot& slot);

		inline const vector_t<node_ref_t>& nodes() const { return m_nodes; }
		inline vector_t<node_ref_t>& nodes() { return m_nodes; }

		inline const vector_t<slink>& links() const { return m_links; }
		inline vector_t<slink>& links() { return m_links; }

		inline id_t master_node() const { return m_master_node; }

	private:
		umap_t<id_t, unsigned> m_nodes_lookup_table;
		umap_t<id_t, unsigned> m_links_lookup_table;
		vector_t<node_ref_t> m_nodes;
		vector_t<slink> m_links;
		cid_generator m_generator;
		id_t m_master_node = C_INVALID_ID;

		RTTR_ENABLE();
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename T, typename... ARGS>
	node_ref_t cmaterial_graph::create_node(ARGS&&... args)
	{
		const auto id = m_generator.next_node_id();

		auto node = std::make_shared<T>(id, this, std::forward<ARGS>(args)...);

		auto idx = SCAST(unsigned, m_nodes.size());

		m_nodes.push_back(node);

		m_nodes_lookup_table[id] = idx;

		if (core::string_utils::compare(node->type().data(), "master_node"))
		{
			m_master_node = id;
		}

		return node;
	}

} //- editor
