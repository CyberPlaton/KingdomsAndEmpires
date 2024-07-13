#pragma once
#include "editor_dock_base.hpp"
#include "asset_browser/asset_browser.hpp"
#include "../elements/editor_element_tab_bar.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	class cbottom_panel final : public clayer_base
	{
	public:
		cbottom_panel(scontext& ctx) : clayer_base(ctx) {};
		~cbottom_panel() = default;

		bool init() override;
		void shutdown() override;
		void on_ui_render() override;

	private:
		vector_t<ui::ctab_bar::sitem> m_tab_bar_items;
		vector_t<layer_ref_t> m_elements;
		unsigned m_active = 0;
	};

} //- editor