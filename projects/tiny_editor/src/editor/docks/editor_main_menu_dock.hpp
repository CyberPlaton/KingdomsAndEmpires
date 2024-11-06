#pragma once
#include "editor_dock_base.hpp"
#include "../editor_element_stack_system.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	class cmain_menu final : public clayer_base
	{
	public:
		cmain_menu(scontext& ctx) : clayer_base("##main_menu_dock", ctx) {};
		~cmain_menu() = default;

		bool init() override final;
		void shutdown() override final;
		void on_ui_render() override final;
		void on_world_render() override final;
		void on_update(float dt) override final;
		void on_post_update(float dt) override final;

	private:
		celement_stack_system m_elements_stack;

	private:
		void show_create_project_dialog(bool* open);
		void show_create_world_dialog(bool *open);
		void show_project_settings(bool *open);
	};

} //- editor