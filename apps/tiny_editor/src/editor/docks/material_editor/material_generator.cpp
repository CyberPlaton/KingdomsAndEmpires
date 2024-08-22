#include "material_generator.hpp"

namespace editor
{
	namespace
	{
	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	cmaterial_generator::cmaterial_generator(const material_graph_ref_t& recipe) : m_material_graph(recipe)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	memory_ref_t cmaterial_generator::generate()
	{
		const auto& nodes = m_material_graph->nodes();

		for (const auto& node : nodes)
		{
			auto id = node->id();

			if (algorithm::bit_check(node->stage(), material_generation_stage_main_code))
			{
				m_stage.m_code_stage_nodes.push_back(id);

				if (algorithm::bit_check(node->stage(), material_generation_stage_header))
				{
					m_stage.m_header_stage_nodes.push_back(node->id());
				}
				if (algorithm::bit_check(node->stage(), material_generation_stage_declarations))
				{
					m_stage.m_decls_stage_nodes.push_back(node->id());
				}

				//- Store potential entry node for later sorting
				if (node->input_count() == 0)
				{
					m_stage.m_entry_nodes.push_back(id);
				}
			}
		}

		generate(m_code_buffer, m_stage);

		//- Output vertex shader data to file
		log_debug(fmt::format("Shader:\n'{}'", m_code_buffer.c_str()));

		return {};
	}

	//------------------------------------------------------------------------------------------------------------------------
	memory_ref_t cmaterial_generator::generate(core::cstring_buffer& buffer, sstage& stage)
	{
		core::cstring_buffer temp;

		//- Sort the stage nodes topologically
		auto order = topsort(stage.m_entry_nodes);

		
		for (const auto& id: order)
		{
			auto& node = m_material_graph->node_at(id);

			if (node->emit(temp, material_generation_stage_header))
			{
				buffer.write(temp.c_str());
			}

			temp.clear();
		}

		for (const auto& id: order)
		{
			auto& node = m_material_graph->node_at(id);

			if (node->emit(temp, material_generation_stage_declarations))
			{
				buffer.write(temp.c_str());
			}

			temp.clear();
		}

		for (const auto& id: order)
		{
			auto& node = m_material_graph->node_at(id);

			if (node->emit(temp, stage.m_stage))
			{
				buffer.write(temp.c_str());
			}

			temp.clear();
		}

		return {};
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmaterial_generator::stage_nodes_t cmaterial_generator::topsort(stage_nodes_t& nodes)
	{
		umap_t<id_t, unsigned> indegrees;
		stage_nodes_t sorted;
		const auto nodes_count = nodes.size();

		//- Initialize nodes with their in degree
		for (const auto& node: m_material_graph->nodes())
		{
			indegrees[node->id()] = node->input_count();
		}

		algorithm::reverse(nodes.begin(), nodes.end());

		//- Execute algorithm
		while (!nodes.empty())
		{
			auto& id = nodes.back(); nodes.pop_back();

			sorted.push_back(id);

			auto& node = m_material_graph->node_at(id);

			for (auto idx = 0; idx < node->output_count(); ++idx)
			{
				auto& out_slot = node->output_at_idx(idx);

				if (const auto link = m_material_graph->link_at(out_slot.m_link_id); link.m_id != C_INVALID_ID)
				{
					if (auto to_node = m_material_graph->node_at(link.m_to_node); to_node)
					{
						--indegrees[to_node->id()];

						if (indegrees[to_node->id()] == 0)
						{
							nodes.push_back(to_node->id());
						}
					}
				}
			}
		}

		//- Quick validate
		if (sorted.size() != nodes_count)
		{
			log_error("Topological sorting algorithm failed! Abstain from using the resulting shader code!");
		}
		return sorted;
	}

} //- editor