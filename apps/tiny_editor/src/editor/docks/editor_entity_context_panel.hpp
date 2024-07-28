#pragma once
#include "editor_dock_base.hpp"
#include "../editor_element_stack_system.hpp"
#include "entity_context_menu/entity_context_menu.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	class centity_context_panel final : public clayer_base
	{
	public:
		centity_context_panel(scontext& ctx) : clayer_base(ImGui::GetID("##entity_context_panel"), ctx) {};
		~centity_context_panel() = default;

		bool init() override final;
		void shutdown() override final;
		void on_ui_render() override final;
		void on_world_render() override final;
		void on_update(float dt) override final;
		void on_post_update(float dt) override final;

	private:
		celement_stack_system m_elements_stack;
	};

} //- editor