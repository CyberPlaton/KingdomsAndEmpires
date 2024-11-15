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
		cgeneration_context ctx(m_material_graph.get(), m_code_buffer, m_settings);

		return generate(ctx);
	}

	//------------------------------------------------------------------------------------------------------------------------
	memory_ref_t cmaterial_generator::generate(cgeneration_context& ctx)
	{
		auto result = true;

		//- Sort the stage nodes topologically
		auto order = topsort(ctx.graph()->master_node());

		for (const auto& id : order)
		{
			auto node = ctx.graph()->node_at(id);

			for (slot_idx_t idx = 0; idx < node->input_count(); ++idx)
			{
				node->emit(ctx, idx);
			}
		}

		//- Output vertex shader data to file
		log_debug(fmt::format("Shader:\n'{}'", m_code_buffer.c_str()));

		return {};
	}

	//------------------------------------------------------------------------------------------------------------------------
	vector_t<id_t> cmaterial_generator::topsort(id_t entry)
	{
		umap_t<id_t, unsigned> indegrees;
		vector_t<id_t> sorted;
		vector_t<id_t> nodes({entry});

		//- Initialize nodes with their in degree
		for (const auto& node: m_material_graph->nodes())
		{
			indegrees[node->id()] = node->input_count();
		}

		//- Execute algorithm
		while (!nodes.empty())
		{
			auto& id = nodes.back(); nodes.pop_back();

			sorted.push_back(id);

			auto node = m_material_graph->node_at(id);

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
		return sorted;
	}

} //- editor
