#pragma once
#include "../editor_dock_base.hpp"

namespace editor
{
	//- Not a dock or layer per-se, intended to be used directly by the respective panel.
	//------------------------------------------------------------------------------------------------------------------------
	class cworld_inspector final : public clayer_base
	{
	public:
		cworld_inspector(ccontext& ctx) : clayer_base(ctx) {};
		~cworld_inspector() = default;

		bool init() override;
		void shutdown() override;
		void on_ui_render() override;

	private:

	};

} //- editor