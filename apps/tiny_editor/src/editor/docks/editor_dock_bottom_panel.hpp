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

		bool init() override final;
		void shutdown() override final;
		void on_ui_render() override final;
		void on_world_render() override final;
		void on_update(float dt) override final;
		void on_post_update(float dt) override final;

	private:
		celement_stack_system m_elements_stack;
		vector_t<ui::ctab_bar::sitem> m_tab_bar_items;
		unsigned m_active = 0;
	};

} //- editor