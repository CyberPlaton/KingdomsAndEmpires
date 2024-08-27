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
		core::cstring_buffer m_code_buffer;
		material_graph_ref_t m_material_graph;
		sgeneration_settings m_settings;

	private:
		memory_ref_t generate(cgeneration_context& ctx);
		vector_t<id_t> topsort(id_t entry);
	};

} //- editor