#include "editor_dock_left_panel.hpp"

namespace editor
{
	namespace
	{
		constexpr unsigned C_WIDTH = 25;
		constexpr unsigned C_HEIGHT = 70;
		constexpr std::string_view C_LEFT_PANEL_ID = "##editor_dock_left_panel";
		constexpr auto C_LEFT_PANEL_FLAGS = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	bool cleft_panel::init()
	{
		if (auto inspector = m_elements_stack.push<cworld_inspector>(ctx()); inspector && inspector->init())
		{
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cleft_panel::shutdown()
	{
		m_elements_stack.shutdown();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cleft_panel::on_ui_render()
	{
		if (!ctx().m_docks_enabled)
			return;

		const ImVec2 size = {algorithm::percent_value(C_WIDTH, ctx().m_window_width),
			algorithm::percent_value(C_HEIGHT, ctx().m_window_height) };
		const ImVec2 pos = {0.0f, ctx().m_mainmenu_height};


		ImGui::SetNextWindowSize(size, ImGuiCond_Always);
		ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
		ImGui::Begin(C_LEFT_PANEL_ID.data(), nullptr, C_LEFT_PANEL_FLAGS);

		m_elements_stack.on_ui_render();

		ImGui::End();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cleft_panel::on_world_render()
	{
		m_elements_stack.on_world_render();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cleft_panel::on_update(float dt)
	{
		m_elements_stack.on_update(dt);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cleft_panel::on_post_update(float dt)
	{
		m_elements_stack.on_post_update(dt);
	}

} //- editor