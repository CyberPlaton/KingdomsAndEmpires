#pragma once
#include <core.h>
#include <spritemancer.h>
#include <engine.h>

//- Use macro to declare a node inherited from a number of base nodes
//------------------------------------------------------------------------------------------------------------------------
#define DECLARE_NODE(...) \
RTTR_ENABLE(__VA_ARGS__); \
RTTR_REGISTRATION_FRIEND;

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
	using slot_idx_t = uint8_t;

	constexpr id_t C_INVALID_ID = MAX(id_t);

	namespace detail
	{
		string_t variant_to_string(rttr::variant& var, rttr::type expected);
	}

	namespace scopes
	{

	} //- scopes

	//------------------------------------------------------------------------------------------------------------------------
	enum slot_type : uint8_t
	{
		slot_type_none = 0,
		slot_type_input,
		slot_type_output
	};

	//- Settings for generating a material. Can be platform or language dependent etc. and are used by nodes to generate
	//- code in a special way.
	//------------------------------------------------------------------------------------------------------------------------
	struct sgeneration_settings final
	{

	};

	//- Responsible for providing contextual data during code generation that is required for most nodes.
	//------------------------------------------------------------------------------------------------------------------------
	class cgeneration_context final :
		private core::cnon_copyable,
		private core::cnon_movable
	{
	public:
		class cclosure final
		{
		public:
			cclosure(core::cstring_buffer& root);
			~cclosure() = default;

			core::cstring_buffer& code();
			cclosure& push();
			cclosure& pop(const bool should_combine_hint = true);
			string_t string();
			inline bool empty() const { return m_stack.empty(); }

		private:
			core::stack_t<core::cstring_buffer> m_stack;
			core::cstring_buffer& m_root_scope;
		};


		explicit cgeneration_context(cmaterial_graph* graph, core::cstring_buffer& buffer, const sgeneration_settings& settings);
		~cgeneration_context() = default;


		inline cclosure& closure() { return m_closure; }
		inline cmaterial_graph* graph() const { return m_graph; }
		inline const sgeneration_settings& settings() const { return m_settings; }

	private:
		cclosure m_closure;
		sgeneration_settings m_settings;
		core::cstring_buffer& m_code_buffer;
		cmaterial_graph* m_graph;
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
		rttr::variant m_value_default;
		rttr::type m_expected_value_type;
		id_t m_id = C_INVALID_ID;
		id_t m_link_id = C_INVALID_ID;
		slot_type m_type = slot_type_none;
	};

	//- Interface class for a node that generates a self-contained code snippet to be used in the shader.
	//------------------------------------------------------------------------------------------------------------------------
	class inode
	{
	public:
		virtual void emit(cgeneration_context& ctx, const slot_idx_t idx) = 0;

		virtual vector_t<sslot>& inputs() = 0;
		virtual vector_t<sslot>& outputs() = 0;
		virtual sslot& find_input_slot(stringview_t name) = 0;
		virtual sslot& find_output_slot(stringview_t name) = 0;
		virtual sslot& input_at_idx(slot_idx_t idx) = 0;
		virtual sslot& output_at_idx(slot_idx_t idx) = 0;
		virtual sslot& input_at(id_t id) = 0;
		virtual sslot& output_at(id_t id) = 0;
		virtual unsigned input_count() const = 0;
		virtual unsigned output_count() const = 0;
		virtual string_t input_slot_value_at_idx(cgeneration_context& ctx, const slot_idx_t idx) = 0;

		virtual id_t id() const = 0;
		virtual stringview_t type() const = 0;

		RTTR_ENABLE();
	};

	//- 
	//------------------------------------------------------------------------------------------------------------------------
	class cnode : public inode
	{
	public:
		cnode(const id_t id, cmaterial_graph* graph) :
			m_id(id), m_graph(graph)
		{
		}

		virtual ~cnode() = default;

		template<typename TValueType>
		cnode& create_slot(stringview_t name, slot_type type, TValueType default_value);
		cnode& create_slot(stringview_t name, slot_type type, rttr::type value_type, rttr::variant default_value);

		vector_t<sslot>& inputs() override final { return m_inputs; }
		vector_t<sslot>& outputs() override final { return m_outputs; }
		sslot& find_input_slot(stringview_t name) override final;
		sslot& find_output_slot(stringview_t name) override final;
		sslot& input_at_idx(slot_idx_t idx) override final;
		sslot& output_at_idx(slot_idx_t idx) override final;
		sslot& input_at(id_t id) override final;
		sslot& output_at(id_t id) override final;
		unsigned input_count() const override final;
		unsigned output_count() const override final;
		string_t input_slot_value_at_idx(cgeneration_context& ctx, const slot_idx_t idx) override final;

		id_t id() const override final { return m_id; }
		virtual stringview_t type() const { return {}; }
		inline cmaterial_graph* graph() const { return m_graph; }

	public:
		int m_stage;
		id_t m_id;

	public:
		umap_t<string_t, unsigned> m_slot_name_lookup_table;
		umap_t<id_t, unsigned> m_slot_id_lookup_table;
		vector_t<sslot> m_inputs;
		vector_t<sslot> m_outputs;
		cmaterial_graph* m_graph;

		DECLARE_NODE(inode);
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TValueType>
	cnode& cnode::create_slot(stringview_t name, slot_type type, TValueType default_value)
	{
		return create_slot(name, type, rttr::type::get<TValueType>(), rttr::variant(default_value));
	}

} //- editor