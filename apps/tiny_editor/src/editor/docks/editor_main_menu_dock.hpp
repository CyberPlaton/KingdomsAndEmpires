#pragma once
#include "editor_dock_base.hpp"
#include "material_editor/editor_material_editor.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	class cmain_menu final : public clayer_base
	{
	public:
		cmain_menu(scontext& ctx) : clayer_base(ImGui::GetID("##main_menu_dock"), ctx) {};
		~cmain_menu() = default;

		bool init() override final;
		void shutdown() override final;
		void on_update(float dt) override final;
		void on_ui_render() override final;

	private:
		layer_ref_t m_material_editor;
	};

} //- editor