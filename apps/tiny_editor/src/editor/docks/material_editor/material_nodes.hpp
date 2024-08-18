#pragma once
#include "material_common.hpp"

namespace editor
{
	//- A constant node generally has no inputs and one (or more) outputs with a constant value.
	//------------------------------------------------------------------------------------------------------------------------
	class cnode_constant final : public cnode
	{
	public:
		template<typename TValueType>
		cnode_constant(const id_t node_id, cmaterial_graph* graph, const int stage, stringview_t name, const TValueType default = (TValueType)0);
		~cnode_constant() = default;

		bool emit(core::cstring_buffer& out, material_generation_stage generation_stage) override final;
		stringview_t output_name() const override final;

	private:
		string_t m_output_name;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TValueType>
	cnode_constant::cnode_constant(const id_t node_id, cmaterial_graph* graph, const int stage, stringview_t name,
		const TValueType default /*= (TValueType)0*/) : cnode(node_id, graph, stage)
	{
		push_slot<TValueType>(cid_generator::retrieve_slot_id(id(), slot_type_output, 0), name, slot_type_output, default);
		m_output_name = fmt::format("constant_{}", id());
	}

	//------------------------------------------------------------------------------------------------------------------------
	class cadd_node final : public cnode
	{
	public:
		cadd_node(const id_t node_id, cmaterial_graph* graph) : cnode(node_id, graph, material_generation_stage_main_code)
		{
			push_slot<float>(cid_generator::retrieve_slot_id(id(), slot_type_input, 0), "a", slot_type_input, 0.0f);
			push_slot<float>(cid_generator::retrieve_slot_id(id(), slot_type_input, 1), "b", slot_type_input, 0.0f);
			push_slot<float>(cid_generator::retrieve_slot_id(id(), slot_type_output, 0), "output", slot_type_output, 0.0f);
			m_output_name = fmt::format("add_result_{}", id());
		}

		bool emit(core::cstring_buffer& out, material_generation_stage generation_stage) override final;
		stringview_t output_name() const override final;

	private:
		string_t m_output_name;
	};

} //- editor