#include "material_nodes.hpp"
#include "material_graph.hpp"

namespace editor
{
	namespace
	{
		//------------------------------------------------------------------------------------------------------------------------
		string_t variant_to_string(rttr::variant& var, rttr::type expected)
		{
			if (rttr::type::get<float>() == expected) return fmt::to_string(var.get_value<float>());
			else if (rttr::type::get<double>() == expected) return fmt::to_string(var.get_value<double>());
			else if (rttr::type::get<int64_t>() == expected) return fmt::to_string(var.get_value<int64_t>());
			else if (rttr::type::get<int32_t>() == expected) return fmt::to_string(var.get_value<int32_t>());
			else if (rttr::type::get<int16_t>() == expected) return fmt::to_string(var.get_value<int16_t>());
			else if (rttr::type::get<int8_t>() == expected) return fmt::to_string(var.get_value<int8_t>());
			else if (rttr::type::get<uint64_t>() == expected) return fmt::to_string(var.get_value<uint64_t>());
			else if (rttr::type::get<uint32_t>() == expected) return fmt::to_string(var.get_value<uint32_t>());
			else if (rttr::type::get<uint16_t>() == expected) return fmt::to_string(var.get_value<uint16_t>());
			else if (rttr::type::get<uint8_t>() == expected) return fmt::to_string(var.get_value<uint8_t>());
			else if (rttr::type::get<char>() == expected) return fmt::to_string(var.get_value<char>());
			else if (rttr::type::get<unsigned char>() == expected) return fmt::to_string(var.get_value<unsigned char>());
			else if (rttr::type::get<bool>() == expected) return fmt::to_string(var.get_value<bool>());
			else if (rttr::type::get<string_t>() == expected) return var.get_value<string_t>();
			else if (rttr::type::get<stringview_t>() == expected) return var.get_value<stringview_t>().data();
			else if (rttr::type::get<const char*>() == expected) return var.get_value<const char*>();

			CORE_ASSERT(false, "Invalid operation. Unknown shader value variant type!");
			return {};
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	stringview_t cnode_constant::output_name() const
	{
		return m_output_name;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cnode_constant::emit(core::cstring_buffer& out, material_generation_stage generation_stage)
	{
		if (algorithm::bit_check(stage(), generation_stage))
		{
			const auto& slot = output_at_idx(0);

			//- Get value of constant or default value if nothing is set
			rttr::variant v = slot.m_value.is_valid() ? slot.m_value : slot.m_value_default;

			auto s = fmt::format("{} {} = {};\n",
				slot.m_expected_value_type.get_name().data(),
				output_name().data(),
				variant_to_string(v, slot.m_expected_value_type));

			out.write(s);

			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cadd_node::emit(core::cstring_buffer& out, material_generation_stage generation_stage)
	{
		if (algorithm::bit_check(stage(), generation_stage))
		{
			auto& output = output_at_idx(0);
			auto& in0 = input_at_idx(0);
			auto& in1 = input_at_idx(1);

			//- TODO: If an input slot is not assigned to anything, we ought to use
			//- the default values provided
			auto source0 = graph()->source_node_at(in0);
			auto source1 = graph()->source_node_at(in1);

			auto s = fmt::format("{} {} = {} + {};\n",
				output.m_expected_value_type.get_name().data(),
				output_name().data(),
				source0->output_name().data(),
				source1->output_name().data());

			out.write(s);

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