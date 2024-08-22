#pragma once
#include "material_graph.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	class cmaterial_generator final : core::cnon_copyable
	{
	public:
		explicit cmaterial_generator(const material_graph_ref_t& recipe);
		~cmaterial_generator() = default;

		memory_ref_t generate();

	private:
		using stage_nodes_t = vector_t<id_t>;

		struct sstage final
		{
			stage_nodes_t m_header_stage_nodes;
			stage_nodes_t m_decls_stage_nodes;
			stage_nodes_t m_code_stage_nodes;
			vector_t<id_t> m_entry_nodes;	//- To be sorted by topsort
			material_generation_stage m_stage;
		};

		core::cstring_buffer m_code_buffer;
		material_graph_ref_t m_material_graph;
		sstage m_stage;

	private:
		memory_ref_t generate(core::cstring_buffer& buffer, sstage& stage);
		stage_nodes_t topsort(stage_nodes_t& nodes);
	};

} //- editor