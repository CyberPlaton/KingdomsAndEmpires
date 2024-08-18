#pragma once
#include <core.h>
#include <spritemancer.h>
#include <engine.h>

namespace editor
{
	class cmaterial_graph;
	class inode;

} //- editor

using material_graph_ref_t = ref_t<editor::cmaterial_graph>;
using node_ref_t = ref_t<editor::inode>;

namespace editor
{
	using id_t = unsigned;

	constexpr id_t C_INVALID_ID = MAX(id_t);

	//------------------------------------------------------------------------------------------------------------------------
	enum material_generation_stage : uint8_t
	{
		material_generation_stage_none			= 0,
		material_generation_stage_header		= BIT(1),
		material_generation_stage_declarations	= BIT(2),
		material_generation_stage_main_code		= BIT(3),
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum slot_type : uint8_t
	{
		slot_type_none = 0,
		slot_type_input,
		slot_type_output
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct slink final
	{
		id_t m_id			= C_INVALID_ID;
		id_t m_from_slot	= C_INVALID_ID;
		id_t m_from_node	= C_INVALID_ID;
		id_t m_to_slot		= C_INVALID_ID;
		id_t m_to_node		= C_INVALID_ID;
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sslot final
	{
		sslot(rttr::type type) : m_expected_value_type(type) {}

		string_t m_name;
		rttr::variant m_value;
		rttr::variant m_value_default;
		rttr::type m_expected_value_type;
		id_t m_id			= C_INVALID_ID;
		id_t m_link_id		= C_INVALID_ID;
		slot_type m_type	=slot_type_none;
	};

	//- Interface class for a node that generates a self-contained code snippet to be used in the shader.
	//------------------------------------------------------------------------------------------------------------------------
	class inode
	{
	public:
		virtual bool emit(core::cstring_buffer& out, material_generation_stage generation_stage) = 0;
		virtual vector_t<sslot>& inputs() = 0;
		virtual vector_t<sslot>& outputs() = 0;
		virtual sslot& find_input_slot(stringview_t name) = 0;
		virtual sslot& find_output_slot(stringview_t name) = 0;
		virtual sslot& input_at(const unsigned idx) = 0;
		virtual sslot& output_at(const unsigned idx) = 0;
		virtual unsigned input_count() const = 0;
		virtual unsigned output_count() const = 0;

		virtual int stage() const = 0;
		virtual id_t id() const = 0;
		virtual cmaterial_graph* graph() const = 0;
	};

	//- 
	//------------------------------------------------------------------------------------------------------------------------
	class cnode : public inode
	{
	public:
		cnode(const id_t id, cmaterial_graph* graph, const int stage = 0) :
			m_id(id), m_stage(stage), m_graph(graph)
		{
		}

		virtual ~cnode() = default;

		template<typename TValueType>
		cnode& push_slot(stringview_t name, slot_type type, TValueType default_value);

		vector_t<sslot>& inputs() override final { return m_inputs; }
		vector_t<sslot>& outputs() override final { return m_outputs; }
		sslot& find_input_slot(stringview_t name) override final;
		sslot& find_output_slot(stringview_t name) override final;
		sslot& input_at(const unsigned idx) override final;
		sslot& output_at(const unsigned idx) override final;
		unsigned input_count() const override final;
		unsigned output_count() const override final;

		int stage() const override final { return m_stage; }
		id_t id() const override final { return m_id; }
		cmaterial_graph* graph() const override final { return m_graph; }

	public:
		int m_stage;
		id_t m_id;

	public:
		umap_t<string_t, unsigned> m_slot_lookup_table;
		vector_t<sslot> m_inputs;
		vector_t<sslot> m_outputs;
		cmaterial_graph* m_graph;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TValueType>
	cnode& cnode::push_slot(stringview_t name, slot_type type, TValueType default_value)
	{
		sslot slot(rttr::type::get<TValueType>());

		slot.m_expected_value_type = rttr::type::get<TValueType>();
		slot.m_name.assign(name.data());
		slot.m_id = SCAST(unsigned, m_inputs.size());
		slot.m_type = type;
		slot.m_value_default = rttr::variant(default_value);
		/*slot.m_value = slot.m_value_default;*/

		if (type == slot_type_input)
		{
			m_inputs.push_back(slot);
		}
		else
		{
			m_outputs.push_back(slot);
		}

		m_slot_lookup_table[name.data()] = slot.m_id;

		return *this;
	}

} //- editor