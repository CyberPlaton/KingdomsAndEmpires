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
		cnode_constant(const id_t id, cmaterial_graph* graph, stringview_t name, const TValueType default = (TValueType)0);
		~cnode_constant() = default;

		void emit(cgeneration_context& ctx, const slot_idx_t idx) override final;
		stringview_t type() const override final { constexpr stringview_t C_TYPE = "constant"; return C_TYPE; }

	private:
		core::cany m_value;

		DECLARE_NODE(cnode);
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TValueType>
	cnode_constant::cnode_constant(const id_t id, cmaterial_graph* graph, stringview_t name,
		const TValueType default /*= (TValueType)0*/) : cnode(id, graph)
	{
		create_slot<TValueType>(name, slot_type_output, default);
	}

	//------------------------------------------------------------------------------------------------------------------------
	class cnode_function final : public cnode
	{
	public:
		enum function_type : uint8_t
		{
			function_type_none = 0,
			function_type_binary_operation,
			function_type_function_call,
		};

		struct sfunction_data
		{
			vector_t<string_t> m_function_operands;
			string_t m_function_name;
			string_t m_function_output_type;
			function_type m_function_type = function_type_none;
		};

		cnode_function(const id_t id, cmaterial_graph* graph, const sfunction_data& data);

		void emit(cgeneration_context& ctx, const slot_idx_t idx) override final;
		stringview_t type() const override final { constexpr stringview_t C_TYPE = "function"; return C_TYPE; }

	private:
		sfunction_data m_data;

		DECLARE_NODE(cnode);
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cnode_master final : public cnode
	{
	public:
		cnode_master(const id_t id, cmaterial_graph* graph);

		void emit(cgeneration_context& ctx, const slot_idx_t idx) override final;
		stringview_t type() const override final { constexpr stringview_t C_TYPE = "master_node"; return C_TYPE; };
	};

} //- editor