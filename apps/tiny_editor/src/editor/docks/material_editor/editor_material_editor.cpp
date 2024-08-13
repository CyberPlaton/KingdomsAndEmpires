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

			for (auto i = 0; i < 10; ++i)
			{
				ImNodes::BeginNode(i);

				ImGui::Text(fmt::format("Node '{}'", i).data());

				//- Inputs
				ImNodes::BeginPin(calcPinId(i, 0), ImNodes::PinKind::Input);
				ImGui::Text(fmt::format("Input Pin '{}'", calcPinId(i, 0)).data());
				ImNodes::EndPin();

				//- Outputs
				ImNodes::BeginPin(calcPinId(i, 1), ImNodes::PinKind::Output);
				ImGui::Text(fmt::format("Output Pin '{}'", calcPinId(i, 1)).data());
				ImNodes::EndPin();

				ImNodes::EndNode();
			}

			for (auto i = 0; i < 10; ++i)
			{
				if (i < 5)
				{
					ImNodes::Link(i, calcPinId(i + 1, 1), calcPinId(i, 0));
				}
				else
				{
					ImNodes::Flow(i);
				}
			}


			ImNodes::End();
		}
		ImGui::End();
	}

} //- editor