#include "editor_dock_right_panel.hpp"
#include "../src/spritemancer/imgui_integration/imgui/imgui.h"

namespace editor
{
	namespace
	{
		constexpr unsigned C_WIDTH = 25;
		constexpr unsigned C_HEIGHT = 70;
		constexpr std::string_view C_RIGHT_PANEL_ID = "##editor_right_bottom_panel";
		constexpr auto C_RIGHT_PANEL_FLAGS = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	bool cright_panel::init()
	{
		m_inspector = std::make_shared<centity_inspector>(ctx());
		return m_inspector->init();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cright_panel::shutdown()
	{
		m_inspector->shutdown();
		m_inspector.reset();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cright_panel::on_ui_render()
	{
		if(!ctx().m_docks_enabled)
			return;

		const ImVec2 size = { algorithm::percent_value(C_WIDTH, ctx().window_width()),
			algorithm::percent_value(C_HEIGHT, ctx().window_height()) };
		const ImVec2 pos = { ctx().window_width() - algorithm::percent_value(C_WIDTH, ctx().window_width()),
			ctx().m_mainmenu_height };

		ImGui::SetNextWindowSize(size, ImGuiCond_Always);
		ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
		ImGui::Begin(C_RIGHT_PANEL_ID.data(), nullptr, C_RIGHT_PANEL_FLAGS);
		m_inspector->on_ui_render();
		ImGui::End();

		const auto& world = ecs::cworld_manager::instance().active();
		const auto& entities = world.em().entities();
		auto e = entities[0];

		if (ImGui::Begin("Component UI test"))
		{
			for (const auto& c : world.em().components(e))
			{
				if (ImGui::CollapsingHeader(c.c_str()))
				{
					auto type = rttr::type::get_by_name(c);

					if (auto m = type.get_method(ecs::detail::C_COMPONENT_SHOW_UI_FUNC_NAME); m.is_valid())
					{
						m.invoke({}, e);
					}
				}
			}
		}
		ImGui::End();
	}

} //- editor