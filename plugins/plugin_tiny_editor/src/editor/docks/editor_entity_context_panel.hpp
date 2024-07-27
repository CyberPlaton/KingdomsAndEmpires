#pragma once
#include "editor_dock_base.hpp"
#include "entity_context_menu/entity_context_menu.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	class EDITOR_API centity_context_panel final : public clayer_base
	{
	public:
		centity_context_panel(scontext& ctx) : clayer_base(ctx) {};
		~centity_context_panel() = default;

		bool init() override;
		void shutdown() override;
		void on_ui_render() override;

	private:
	};

} //- editor