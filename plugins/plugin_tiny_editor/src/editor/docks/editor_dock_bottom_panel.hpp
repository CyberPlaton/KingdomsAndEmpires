#pragma once
#include "editor_dock_base.hpp"
#include "asset_browser/asset_browser.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	class cbottom_panel final : public clayer_base
	{
	public:
		cbottom_panel(ccontext& ctx) : clayer_base(ctx) {};
		~cbottom_panel() = default;

		bool init() override;
		void shutdown() override;
		void on_ui_render() override;

	private:
		ref_t<casset_browser> m_browser;
	};

} //- editor