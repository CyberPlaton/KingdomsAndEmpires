#include "world_inspector.hpp"

namespace editor
{
	namespace
	{
		constexpr std::string_view C_BOTTOM_PANEL_ID = "##editor_world_inspector";
		constexpr auto C_WORLD_INSPECTOR_CHILD_FLAGS = ImGuiChildFlags_Border;
		constexpr auto C_WORLD_INSPECTOR_FLAGS = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar;

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	bool cworld_inspector::init()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld_inspector::shutdown()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld_inspector::on_ui_render()
	{
		auto& world_manager = ecs::cworld_manager::instance();

		if(!world_manager.has_active())
			return;

		//const auto& entities = world_manager.active().em().entities();

		ImGui::BeginChild(C_BOTTOM_PANEL_ID.data(), {0.0f, 0.0f}, C_WORLD_INSPECTOR_CHILD_FLAGS, C_WORLD_INSPECTOR_FLAGS);

		show_menubar();

		switch (m_state.m_view_type)
		{
		default:
		case world_view_type_hierarchy:
		{
			show_world_hierarchy_view(world_manager.active().world());
			break;
		}
		case world_view_type_layered:
		{
			show_rendering_layer_view(world_manager.active().world());
			break;
		}
		}

		ImGui::EndChild();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld_inspector::show_menubar()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("View Type"))
			{
				if (ImGui::MenuItem("Hierarchy", nullptr, m_state.m_view_type == world_view_type_hierarchy))
				{
					m_state.m_view_type = world_view_type_hierarchy;
				}
				if (ImGui::MenuItem("Layered", nullptr, m_state.m_view_type == world_view_type_layered))
				{
					m_state.m_view_type = world_view_type_layered;
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld_inspector::show_rendering_layer_view(const flecs::world& world)
	{
		//- TODO: use cashing and recreate layered view only if something happened like a new layer added or removed
		//- TODO: how and where do we query for the currently created layers? sm does not care about it, so it should be our
		//- responsibility to create new ones and name them

		m_state.m_layered_view.clear();

		world.each([&](flecs::entity e, const ecs::ssprite& sprite, const ecs::sidentifier& identifier)
			{
				m_state.m_layered_view[sprite.m_layer].push_back(e);
			});
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld_inspector::show_world_hierarchy_view(const flecs::world& world)
	{
		//- TODO: use caching and recreate hierarchy view only if something happened

		world.each([&](flecs::entity e, const ecs::shierarchy& hierarchy, const ecs::sidentifier& identifier)
			{
			});
	}

} //- editor