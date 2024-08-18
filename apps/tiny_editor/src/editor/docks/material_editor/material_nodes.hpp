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
		cnode_constant(const id_t id, cmaterial_graph* graph, const int stage, stringview_t name, const TValueType default = (TValueType)0);
		~cnode_constant() = default;

		bool emit(core::cstring_buffer& out, material_generation_stage generation_stage) override final;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TValueType>
	cnode_constant::cnode_constant(const id_t id, cmaterial_graph* graph, const int stage, stringview_t name,
		const TValueType default /*= (TValueType)0*/) : cnode(id, graph, stage)
	{
		push_slot<TValueType>(name, slot_type_output, default);
	}

	//------------------------------------------------------------------------------------------------------------------------
	class cadd_node final : public cnode
	{
	public:
		cadd_node(const id_t id, cmaterial_graph* graph) : cnode(id, graph, material_generation_stage_main_code)
		{
			push_slot<float>("a", slot_type_input, 0.0f);
			push_slot<float>("b", slot_type_input, 0.0f);
			push_slot<float>("output", slot_type_output, 0.0f);
		}

		bool emit(core::cstring_buffer& out, material_generation_stage generation_stage) override final;
	};

} //- editor