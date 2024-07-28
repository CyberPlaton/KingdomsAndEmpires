#pragma once
#include "editor_dock_base.hpp"
#include "../editor_element_stack_system.hpp"
#include "entity_inspector/entity_inspector.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	class cright_panel final : public clayer_base
	{
	public:
		cright_panel(scontext& ctx) : clayer_base(ImGui::GetID("##right_panel"), ctx) {};
		~cright_panel() = default;

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