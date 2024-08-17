#pragma once
#include "material_common.hpp"

namespace editor
{
	//- Class containing editable representation of a shader in form of nodes, links and data that is used
	//- to generate valid and functioning shader code understandable by the runtime engine.
	//------------------------------------------------------------------------------------------------------------------------
	class cmaterial_graph final
	{
	public:
		cmaterial_graph();
		~cmaterial_graph();

		template<typename T, typename... ARGS>
		node_ref_t create_node(const id_t id, ARGS&&... args);

		void create_link(const id_t id, const id_t from_node_id, const id_t to_node_id, const id_t from_slot_id, const id_t to_slot_id);

		const node_ref_t& node_at(id_t id) const;
		node_ref_t& node_at(id_t id);

		const slink& link_at(id_t id) const;
		slink& link_at(id_t id);

		inline const vector_t<node_ref_t>& nodes() const { return m_nodes; }
		inline vector_t<node_ref_t>& nodes() { return m_nodes; }

		inline const vector_t<slink>& links() const { return m_links; }
		inline vector_t<slink>& links() { return m_links; }

	private:
		umap_t<id_t, unsigned> m_nodes_lookup_table;
		umap_t<id_t, unsigned> m_links_lookup_table;
		vector_t<node_ref_t> m_nodes;
		vector_t<slink> m_links;

		RTTR_ENABLE();
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename T, typename... ARGS>
	node_ref_t cmaterial_graph::create_node(const id_t id, ARGS&&... args)
	{
		auto node = std::make_shared<T>(id, std::forward<ARGS>(args)...);

		auto idx = SCAST(unsigned, m_nodes.size());

		m_nodes.push_back(node);

		m_nodes_lookup_table[id] = idx;

		return node;
	}

} //- editor
