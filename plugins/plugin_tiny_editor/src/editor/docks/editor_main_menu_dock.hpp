#pragma once
#include "editor_dock_base.hpp"
#include "material_editor/editor_material_editor.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	class EDITOR_API cmain_menu final : public clayer_base
	{
	public:
		cmain_menu(scontext& ctx) : clayer_base(ctx) {};
		~cmain_menu() = default;

		bool init() override final;
		void shutdown() override final;
		void on_update(float dt) override final;
		void on_ui_render() override final;

	private:
		layer_ref_t m_material_editor;
	};

} //- editor