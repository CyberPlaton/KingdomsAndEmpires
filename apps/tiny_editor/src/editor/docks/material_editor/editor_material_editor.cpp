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

		auto add = m_graph->create_node<cnode_function>(1, C_ADD_FUNCTION_DATA);
		auto constant_a = m_graph->create_node<cnode_constant, float>(2, material_generation_stage_main_code, "A", 1.0f);
		auto constant_b = m_graph->create_node<cnode_constant, float>(3, material_generation_stage_main_code, "B", 1.0f);

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
				imnodes::clink def(link.m_id, link.m_from_slot, link.m_to_slot);

				def.draw();
			}

// 			for (auto i = 1; i < 15; ++i)
// 			{
// 				ImNodes::BeginNode(i);
// 
// 				ImGui::Text(fmt::format("Node '{}'", i).data());
// 
// 				//- Inputs
// 				imnodes::cpin in_pin(calcPinId(i, 0));
// 				in_pin.type(imnodes::cpin::pin_type_input).begin();
// 				ImGui::Text(fmt::format("Input Pin '{}'", calcPinId(i, 0)).data());
// 				in_pin.end();
// 				
// 				//- Outputs
// 				imnodes::cpin out_pin(calcPinId(i, 1));
// 				out_pin.type(imnodes::cpin::pin_type_output).begin();
// 				ImGui::Text(fmt::format("Output Pin '{}'", calcPinId(i, 0)).data());
// 				out_pin.end();
// 
// 				ImNodes::EndNode();
// 			}
// 
// 			for (auto i = 1; i < 15; ++i)
// 			{
// 				imnodes::clink link(i, calcPinId(i + 1, 1), calcPinId(i, 0));
// 				link.flow(imnodes::clink::link_flow_direction_forward, i < 5)
// 					.flow_style(imnodes::clink::link_flow_duration, 1.0f)
// 					.draw();
// 			}

			ImNodes::End();
		}
		ImGui::End();
	}

} //- editor