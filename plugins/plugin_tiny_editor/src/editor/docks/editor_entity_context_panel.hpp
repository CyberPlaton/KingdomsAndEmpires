#pragma once
#include "editor_dock_base.hpp"
#include "entity_context_menu/entity_context_menu.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	class centity_context_panel final : public clayer_base
	{
	public:
		centity_context_panel(ccontext& ctx) : clayer_base(ctx) {};
		~centity_context_panel() = default;

		bool init() override;
		void shutdown() override;
		void on_ui_render() override;

	private:
	};

} //- editor