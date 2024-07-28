#include "editor_material_editor.hpp"
/*#include "../../nodes/editor_basic_node.hpp"*/

namespace editor
{
	namespace
	{
	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	cmaterial_editor::cmaterial_editor(scontext& ctx) :
		clayer_base(ctx)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmaterial_editor::~cmaterial_editor()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmaterial_editor::init()
	{
		//- Set some style variables
// 		auto& style = nodes::cstyle().style();
// 		style.PinLineThickness = 1.0f;
// 		style.GridSpacing = 8.0f;
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
		}
		ImGui::End();
	}

} //- editor