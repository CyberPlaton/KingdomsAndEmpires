#pragma once
#include "material_common.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	class cconstant_node final : public cnode
	{
	public:
		cconstant_node(const id_t id) : cnode(id)
		{
			m_stage |= material_generation_stage_vertex_code;
			m_stage |= material_generation_stage_pixel_code;
			push_slot<float>("output", slot_type_output, 0.0f);
		}

		bool emit(core::cstring_buffer& out, material_generation_stage stage) override final
		{
			if (stage == material_generation_stage_pixel_code ||
				stage == material_generation_stage_vertex_code)
			{
				out.write(fmt::format("float {} = {};\n",
					output_at(0).m_name,
					output_at(0).m_value.get_value<float>()));

				return true;
			}
			return false;
		}
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cadd_node final : public cnode
	{
	public:
		cadd_node(const id_t id) : cnode(id)
		{
			m_stage |= material_generation_stage_vertex_code;
			m_stage |= material_generation_stage_pixel_code;
			push_slot<float>("a", slot_type_input, 0.0f);
			push_slot<float>("b", slot_type_input, 0.0f);
			push_slot<float>("output", slot_type_output, 0.0f);
		}

		bool emit(core::cstring_buffer& out, material_generation_stage stage) override final
		{
			if (stage == material_generation_stage_pixel_code ||
				stage == material_generation_stage_vertex_code)
			{
				out.write(fmt::format("float {} = {} + {};\n",
					output_at(0).m_name,
					input_at(0).m_value.get_value<float>(),
					input_at(1).m_value.get_value<float>()));

				return true;
			}
			return false;
		}
	};

} //- editor