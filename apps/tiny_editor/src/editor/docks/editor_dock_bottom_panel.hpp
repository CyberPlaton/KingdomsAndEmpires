#pragma once
#include "editor_dock_base.hpp"
#include "asset_browser/asset_browser.hpp"
#include "../elements/editor_element_tab_bar.hpp"
#include "../editor_element_stack_system.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	class cbottom_panel final : public clayer_base
	{
	public:
		cbottom_panel(scontext& ctx) : clayer_base(ImGui::GetID("##bottom_panel"), ctx) {};
		~cbottom_panel() = default;

		bool init() override;
		void shutdown() override;
		void on_ui_render() override;
		void on_world_render() override;
		void on_update(float dt) override;
		void on_post_update(float dt) override;

	private:
		celement_stack_system m_elements_stack;
		vector_t<ui::ctab_bar::sitem> m_tab_bar_items;
		vector_t<layer_ref_t> m_elements;
		unsigned m_active = 0;
	};

} //- editor