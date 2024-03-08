#pragma once
#include "../editor_dock_base.hpp"

namespace editor
{
	//- Not a dock or layer per-se, intended to be used directly by the respective panel.
	//------------------------------------------------------------------------------------------------------------------------
	class casset_browser final : public clayer_base
	{
	public:
		casset_browser(ccontext& ctx) : clayer_base(ctx) {};
		~casset_browser() = default;

		bool init() override;
		void shutdown() override;
		void on_ui_render() override;

	private:

	};

} //- editor