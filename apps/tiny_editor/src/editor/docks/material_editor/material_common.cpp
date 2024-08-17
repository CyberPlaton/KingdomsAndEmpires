#include "material_common.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	sslot& cnode::find_input_slot(stringview_t name)
	{
		return input_at(m_slot_lookup_table.at(name.data()));
	}

	//------------------------------------------------------------------------------------------------------------------------
	sslot& cnode::find_output_slot(stringview_t name)
	{
		return output_at(m_slot_lookup_table.at(name.data()));
	}

	//------------------------------------------------------------------------------------------------------------------------
	sslot& cnode::input_at(const unsigned idx)
	{
		return m_inputs[idx];
	}

	//------------------------------------------------------------------------------------------------------------------------
	sslot& cnode::output_at(const unsigned idx)
	{
		return m_outputs[idx];
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