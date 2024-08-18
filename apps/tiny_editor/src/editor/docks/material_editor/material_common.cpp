#include "material_common.hpp"

namespace editor
{
	namespace
	{
		constexpr auto C_NODE_INPUTS_OUTPUTS_COUNT_MAX = 16;

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	cid_generator::cid_generator(const id_t node_seed /*= 0*/, const id_t link_seed /*= 0*/) :
		m_next_node_id(node_seed), m_next_link_id(link_seed)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cid_generator::~cid_generator()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	id_t cid_generator::generate_node_id()
	{
		return ++m_next_node_id;
	}

	//------------------------------------------------------------------------------------------------------------------------
	id_t cid_generator::retrieve_slot_id(const id_t node_id, slot_type type, unsigned idx)
	{
		switch (type)
		{
		case slot_type_input: return node_id + idx + 1;
		case slot_type_output: return node_id + (idx + 1) + C_NODE_INPUTS_OUTPUTS_COUNT_MAX;
		default: return C_INVALID_ID;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	id_t cid_generator::generate_link_id()
	{
		return ++m_next_link_id;
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
	sslot& cnode::input_at_idx(idx_t idx)
	{
		return m_inputs[idx];
	}

	//------------------------------------------------------------------------------------------------------------------------
	sslot& cnode::output_at_idx(idx_t idx)
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

} //- editor