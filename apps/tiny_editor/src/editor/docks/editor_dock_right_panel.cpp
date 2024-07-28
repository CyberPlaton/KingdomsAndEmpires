#include "editor_dock_right_panel.hpp"

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
		if (auto inspector = m_elements_stack.push<centity_inspector>(ctx()); inspector && inspector->init())
		{
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cright_panel::shutdown()
	{
		m_elements_stack.shutdown();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cright_panel::on_ui_render()
	{
		if(!ctx().m_docks_enabled)
			return;

		const ImVec2 size = { algorithm::percent_value(C_WIDTH, ctx().m_window_width),
			algorithm::percent_value(C_HEIGHT, ctx().m_window_width) };
		const ImVec2 pos = { ctx().m_window_width - algorithm::percent_value(C_WIDTH, ctx().m_window_width),
			ctx().m_mainmenu_height };

		ImGui::SetNextWindowSize(size, ImGuiCond_Always);
		ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
		ImGui::Begin(C_RIGHT_PANEL_ID.data(), nullptr, C_RIGHT_PANEL_FLAGS);

		m_elements_stack.on_ui_render();

		ImGui::End();

		if (ecs::cworld_manager::instance().has_active())
		{
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
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cright_panel::on_world_render()
	{
		m_elements_stack.on_world_render();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cright_panel::on_update(float dt)
	{
		m_elements_stack.on_update(dt);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cright_panel::on_post_update(float dt)
	{
		m_elements_stack.on_post_update(dt);
	}

} //- editor