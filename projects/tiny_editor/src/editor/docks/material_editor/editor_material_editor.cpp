#include "editor_material_editor.hpp"
/*#include "../../nodes/editor_basic_node.hpp"*/

namespace editor
{
	namespace
	{
		static const cnode_function::sfunction_data C_ADD_FUNCTION_DATA = 
		{
			{"float", "float"},
			"+",
			"float",
			cnode_function::function_type_binary_operation
		};

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	cmaterial_editor::cmaterial_editor(scontext& ctx) :
		clayer_base("##material_editor", ctx), m_editor(ImNodes::CreateEditor())
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmaterial_editor::~cmaterial_editor()
	{
		ImNodes::DestroyEditor(m_editor);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmaterial_editor::init()
	{
		m_graph = std::make_shared<cmaterial_graph>();

		auto add = m_graph->create_node<cnode_function>(C_ADD_FUNCTION_DATA);
		auto constant_a = m_graph->create_node<cnode_constant>("A", 1.0f);
		auto constant_b = m_graph->create_node<cnode_constant>("B", 1.0f);
		auto master_node = m_graph->create_node<cnode_master>();

		m_graph->create_link(constant_a->id(), add->id(), 0, 0);
		m_graph->create_link(constant_b->id(), add->id(), 0, 1);

		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial_editor::shutdown()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial_editor::on_update(float dt)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial_editor::on_ui_render()
	{
		static const auto calcPinId = [](const int nodeId, const int type) -> int
			{
				if (type == 0)
				{
					return (nodeId + 1) << 15;
				}
				return (nodeId + 1) << 10;
			};

		if (ImGui::Begin("##MaterialEditor"))
		{
			if (ImGui::Button("Generate"))
			{
				cmaterial_generator generator(m_graph);

				generator.generate();
			}

			CORE_ASSERT(m_editor, "Invalid operation. Node editor context was not created!");

			ImNodes::SetCurrentEditor(m_editor);

			ImNodes::Begin("Node Editor");

			for (auto& node : m_graph->nodes())
			{
				ImNodes::BeginNode(node->id());
				ImGui::TextUnformatted(fmt::format("Node {}", node->id()).data());

				//- Inputs
				for (auto& in : node->inputs())
				{
					imnodes::cpin pin(in.m_id);

					pin
						.type(imnodes::cpin::pin_type_input)
						.begin();

					ImGui::TextUnformatted(fmt::format("{}", in.m_name.data()).data());

					pin.end();
				}

				//- Outputs
				for (auto& out : node->outputs())
				{
					imnodes::cpin pin(out.m_id);

					pin
						.type(imnodes::cpin::pin_type_output)
						.begin();

					ImGui::TextUnformatted(fmt::format("{}", out.m_name.data()).data());

					pin.end();
				}

				ImNodes::EndNode();
			}

			for (auto& link : m_graph->links())
			{
				imnodes::clink(link.m_id, link.m_from_slot, link.m_to_slot).draw();
			}

			ImNodes::End();
		}
		ImGui::End();
	}

} //- editor