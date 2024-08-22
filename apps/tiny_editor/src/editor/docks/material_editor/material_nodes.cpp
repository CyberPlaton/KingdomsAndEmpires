#include "material_nodes.hpp"
#include "material_graph.hpp"

namespace editor
{
	namespace
	{
	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	bool cnode_constant::emit(cgeneration_context& ctx, const slot_idx_t idx)
	{
		const auto& slot = output_at(0);

		//- Get value of constant or default value if nothing is set
		rttr::variant v = !m_value.empty()? m_value.data() : slot.m_value_default;

		auto s = fmt::format("{} {} = {};\n",
			slot.m_expected_value_type.get_name().data(),
			fmt::format("constant_{}", id()).data(),
			detail::variant_to_string(v, slot.m_expected_value_type));

		ctx.closure().code().write(s);

		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cnode_function::cnode_function(const id_t id, cmaterial_graph* graph, const sfunction_data& data) :
		cnode(id, graph), m_data(data)
	{
		//- Automatic setup according to data, first create input slots
		for (auto i = 0; i < m_data.m_function_operands.size(); ++i)
		{
			const auto& operand_type_name = m_data.m_function_operands[i];
			auto operand_type = rttr::type::get_by_name(operand_type_name.data());

			create_slot(fmt::to_string(i), slot_type_input, operand_type, operand_type.create({}));
		}

		//- Create output slots
		auto output_type = rttr::type::get_by_name(m_data.m_function_output_type.data());
		create_slot("out", slot_type_output, output_type, output_type.create({}));
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cnode_function::emit(cgeneration_context& ctx, const slot_idx_t idx)
	{
		string_t s;
		auto& output = output_at(0);

		switch (m_data.m_function_type)
		{
		case function_type_none:
		default:
		{
			return false;
		}
		case function_type_binary_operation:
		{
			//- 
			auto in0 = input_slot_value_at_idx(ctx, 0);
			auto in1 = input_slot_value_at_idx(ctx, 1);

			s = fmt::format("{} {} = {} {} {};\n",
				output.m_expected_value_type.get_name().data(),
				output.m_name,
				in0,
				m_data.m_function_name,
				in1);
			break;
		}
		case function_type_function_call:
		{
			//- Starting of closure
			s = fmt::format("{} {} = {}(",
				output.m_expected_value_type.get_name().data(),
				output.m_name,
				m_data.m_function_name);

			//- Write first parameter
			auto in0 = input_slot_value_at_idx(ctx, 0);

			s = fmt::format("{} {}", s, in0);

			//- Write rest of parameters if there are any left
			for (auto i = 1; i < m_data.m_function_operands.size(); ++i)
			{
				auto ini = input_slot_value_at_idx(ctx, i);

				s = fmt::format("{}, {}", s, ini);
			}

			//- Ending of closure
			s = fmt::format("{});\n", s);
			break;
		}
		}

		if (!s.empty())
		{
			ctx.closure().code().write(s);
			return true;
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	stringview_t cadd_node::output_name() const
	{
		return m_output_name;
	}

} //- editor