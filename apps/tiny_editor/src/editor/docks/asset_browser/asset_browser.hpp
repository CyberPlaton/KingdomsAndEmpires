#pragma once
#include "../editor_dock_base.hpp"

namespace editor
{
	//- Not a dock or layer per-se, intended to be used directly by the respective panel.
	//------------------------------------------------------------------------------------------------------------------------
	class casset_browser final : public clayer_base
	{
	public:
		casset_browser(scontext& ctx) : clayer_base("##asset_browser", ctx) {};
		~casset_browser() = default;

		bool init() override;
		void shutdown() override;
		void on_ui_render() override;

	private:

	};

} //- editor