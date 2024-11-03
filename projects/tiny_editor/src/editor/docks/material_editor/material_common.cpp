#include "material_common.hpp"
#include "material_graph.hpp"

namespace editor
{
	namespace
	{
		constexpr uint64_t C_CLOSURE_BUFFER_SIZE = 256;
		constexpr int C_NODE_SLOT_INPUT_COUNT_MAX = 64;
		constexpr int C_NODE_SLOT_OUTPUT_COUNT_MAX = 8;

	} //- unnamed

	namespace detail
	{
		//------------------------------------------------------------------------------------------------------------------------
		string_t variant_to_string(const rttr::variant& var, rttr::type expected)
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

		//- The only function to be used for generation of a slot id.
		//------------------------------------------------------------------------------------------------------------------------
		id_t derive_slot_id(const editor::inode* node, slot_type type, const slot_idx_t idx)
		{
			CORE_ASSERT(type == slot_type_input ? idx < C_NODE_SLOT_INPUT_COUNT_MAX :
				idx < C_NODE_SLOT_OUTPUT_COUNT_MAX, "Invalid operation. Index is out of bounds!");

			auto b = node->id();

			switch (type)
			{
			case slot_type_input:
			{
				b = b + (1 + idx);
				break;
			}
			case slot_type_output:
			{
				b = b + C_NODE_SLOT_INPUT_COUNT_MAX + (1 + idx);
				break;
			}
			default: return C_INVALID_ID;
			}
			return b;
		}

	} //- detail

	//------------------------------------------------------------------------------------------------------------------------
	cgeneration_context::cclosure::cclosure(core::cstring_buffer& root) : m_root_scope(root)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cgeneration_context::cclosure& cgeneration_context::cclosure::push()
	{
		m_stack.emplace().resize(C_CLOSURE_BUFFER_SIZE);
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cgeneration_context::cclosure& cgeneration_context::cclosure::pop(const bool should_combine_hint /*= true*/)
	{
		CORE_ASSERT(!m_stack.empty(), "Invalid operation. Mismatched 'push/pop' operations, closure stack was empty!");

		auto& buffer = m_stack.top(); m_stack.pop();

		//- Either write to root scope or to next one above
		if (should_combine_hint)
		{
			const char* c_str_buffer = buffer.c_str();

			auto& parent_scope = !m_stack.empty() ? m_stack.top() : m_root_scope;

			parent_scope.write(c_str_buffer);
		}

		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	string_t cgeneration_context::cclosure::string()
	{
		CORE_ASSERT(!m_stack.empty(), "Invalid operation. Closure stack was empty!");

		return { m_stack.top().data() };
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cstring_buffer& cgeneration_context::cclosure::code()
	{
		CORE_ASSERT(!m_stack.empty(), "Invalid operation. Closure stack was empty! Writing to root scope is only allowed on combining.");

		return m_stack.top();
	}

	//------------------------------------------------------------------------------------------------------------------------
	cnode& cnode::create_slot(stringview_t name, slot_type type, rttr::type value_type, rttr::variant default_value)
	{
		sslot slot(value_type);
		slot.m_name.assign(name.data());

		slot.m_id = type == slot_type_input ? detail::derive_slot_id(this, slot_type_input, SCAST(slot_idx_t, m_inputs.size())) :
			detail::derive_slot_id(this, slot_type_output, SCAST(slot_idx_t, m_outputs.size()));

		slot.m_type = type;
		slot.m_value_default = default_value;

		if (type == slot_type_input)
		{
			m_inputs.push_back(slot);
		}
		else
		{
			m_outputs.push_back(slot);
		}

		m_slot_name_lookup_table[name.data()] = slot.m_id;

		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sslot& cnode::find_input_slot(stringview_t name)
	{
		return input_at_idx(m_slot_name_lookup_table.at(name.data()));
	}

	//------------------------------------------------------------------------------------------------------------------------
	sslot& cnode::find_output_slot(stringview_t name)
	{
		return output_at_idx(m_slot_name_lookup_table.at(name.data()));
	}

	//------------------------------------------------------------------------------------------------------------------------
	sslot& cnode::input_at_idx(slot_idx_t idx)
	{
		return m_inputs[idx];
	}

	//------------------------------------------------------------------------------------------------------------------------
	sslot& cnode::output_at_idx(slot_idx_t idx)
	{
		return m_outputs[idx];
	}

	//------------------------------------------------------------------------------------------------------------------------
	sslot& cnode::input_at(id_t id)
	{
		if (const auto it = m_slot_id_lookup_table.find(id); it != m_slot_id_lookup_table.end())
		{
			return m_inputs[it->second];
		}
		static sslot S_INVALID(rttr::type::get<void>());
		return S_INVALID;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sslot& cnode::output_at(id_t id)
	{
		if (const auto it = m_slot_id_lookup_table.find(id); it != m_slot_id_lookup_table.end())
		{
			return m_outputs[it->second];
		}
		static sslot S_INVALID(rttr::type::get<void>());
		return S_INVALID;
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cnode::input_count() const
	{
		return SCAST(unsigned, m_inputs.size());
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cnode::output_count() const
	{
		return SCAST(unsigned, m_outputs.size());
	}

	//------------------------------------------------------------------------------------------------------------------------
	string_t cnode::input_slot_value_at_idx(cgeneration_context& ctx, const slot_idx_t idx)
	{
		string_t result;

		//- Get the source node that is linked to slot at given index
		const auto& slot = m_inputs[idx];

		if (const auto link = graph()->link_at(slot.m_link_id); link.m_id != C_INVALID_ID)
		{
			if (const auto& src_node = graph()->node_at(link.m_from_node); src_node)
			{
				ctx.closure().push();

				src_node->emit(ctx, idx);

				result = ctx.closure().string();

				ctx.closure().pop(!result.empty());

				if (!result.empty())
				{
					//- When we fail to retrieve a value, we fallback to default one
					ctx.closure().push();

					ctx.closure().code().write(detail::variant_to_string(slot.m_value_default, slot.m_expected_value_type));

					result = ctx.closure().string();

					ctx.closure().pop();
				}
			}
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cgeneration_context::cgeneration_context(cmaterial_graph* graph, core::cstring_buffer& buffer, const sgeneration_settings& settings) :
		m_code_buffer(buffer), m_closure({ m_code_buffer }), m_graph(graph), m_settings(settings)
	{
	}

} //- editor