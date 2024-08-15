#include "editor_material_editor.hpp"
/*#include "../../nodes/editor_basic_node.hpp"*/

namespace editor
{
	namespace
	{
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
			CORE_ASSERT(m_editor, "Invalid operation. Node editor context was not created!");

			ImNodes::SetCurrentEditor(m_editor);

			ImNodes::Begin("Node Editor");

			for (auto i = 1; i < 15; ++i)
			{
				ImNodes::BeginNode(i);

				ImGui::Text(fmt::format("Node '{}'", i).data());

				//- Inputs
				imnodes::cpin in_pin(calcPinId(i, 0));
				in_pin.type(imnodes::cpin::pin_type_input).begin();
				ImGui::Text(fmt::format("Input Pin '{}'", calcPinId(i, 0)).data());
				in_pin.end();
				
				//- Outputs
				imnodes::cpin out_pin(calcPinId(i, 1));
				out_pin.type(imnodes::cpin::pin_type_output).begin();
				ImGui::Text(fmt::format("Output Pin '{}'", calcPinId(i, 0)).data());
				out_pin.end();

				ImNodes::EndNode();
			}

			for (auto i = 1; i < 15; ++i)
			{
				imnodes::clink link(i, calcPinId(i + 1, 1), calcPinId(i, 0));
				link.flow(imnodes::clink::link_flow_direction_backward, i < 5)
					.flow_style(imnodes::clink::link_flow_duration, 1.0f)
					.draw();
			}


			ImNodes::End();
		}
		ImGui::End();
	}

} //- editor