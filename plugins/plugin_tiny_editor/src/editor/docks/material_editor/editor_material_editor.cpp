#include "editor_material_editor.hpp"
#include "../../nodes/editor_basic_node.hpp"

namespace editor
{
	namespace
	{
	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	cmaterial_editor::cmaterial_editor(scontext& ctx) :
		clayer_base(ctx), m_ctx(ImNodes::EditorContextCreate())
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmaterial_editor::~cmaterial_editor()
	{
		ImNodes::EditorContextFree(m_ctx);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmaterial_editor::init()
	{
		//- Set some style variables
		auto& style = nodes::cstyle().style();
		style.PinLineThickness = 1.0f;
		style.GridSpacing = 8.0f;
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
		if (ImGui::Begin("##MaterialEditor"))
		{
			ImNodes::BeginNodeEditor();

			ImNodes::EditorContextSet(m_ctx);

			for (auto i = 1; i < 6; ++i)
			{
				nodes::cbasic_node::cscope scope(i);

				auto& settings = scope.settings();

				if (i % 2 == 0)
				{
					settings.color(ImNodesCol_NodeBackground, { 0, 255, 0, 255 });
				}
				else
				{
					settings.color(ImNodesCol_NodeBackground, { 255, 0, 0, 255 });
				}
				settings.header(fmt::format("Node {}", i), ICON_FA_ARROWS_DOWN_TO_PEOPLE,
					"A node I loved so much", "... in truth, I didnt really 'loved' the node");

				//- output pin
				nodes::cpin pin(i);

				pin.type(nodes::pin_type_output)
					.shape(nodes::pin_shape_quad)
					;

				if (i % 2 == 0)
				{
					pin.filled(true)
						.color(ImNodesCol_Pin, { 200, 150, 150, 255 });
				}
				else
				{
					pin.filled(false)
						.color(ImNodesCol_Pin, { 100, 250, 250, 255 });
				}

				pin.begin();

				ImGui::Text(fmt::format("Output '{}'", i).data());

				pin.end();

				//- input pin
				nodes::cpin in_pin(256 + i);

				in_pin.type(nodes::pin_type_input)
					.shape(nodes::pin_shape_triangle)
					;

				if (i % 2 == 0)
				{
					in_pin.filled(true)
						.color(ImNodesCol_Pin, { 115, 115, 25, 255 });
				}
				else
				{
					in_pin.filled(false)
						.color(ImNodesCol_Pin, { 100, 25, 115, 255 });
				}

				in_pin.begin();

				ImGui::Text(fmt::format("Input '{}'", 256 + i).data());

				in_pin.end();
			}

			nodes::clink link(1);

 			link.begin(4, 257);

			ImNodes::EndNodeEditor();
		}
		ImGui::End();
	}

} //- editor