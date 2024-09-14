#include "world_inspector.hpp"
#include "../../elements/editor_element_table.hpp"

namespace editor
{
	namespace
	{
		constexpr auto C_TREE_NODE_BASE_FLAGS = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow;
		constexpr std::string_view C_BOTTOM_PANEL_ID = "##editor_world_inspector";
		constexpr auto C_WORLD_INSPECTOR_CHILD_FLAGS = ImGuiChildFlags_Border;
		constexpr auto C_WORLD_INSPECTOR_FLAGS = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar;

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	bool cworld_inspector::init()
	{
		m_context_menu = std::make_shared<centity_context_menu>(ctx());
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

		if(!world_manager.has_active()) return;

		if (state().m_dirty)
		{
			recreate_snapshot(world_manager.active().world());
		}

		ui::ctable table("##table");

		table
			.size({ 100, 100 })
			.tooltip("Tooltip")
			.options(ui::ctable::options_borders_all | ui::ctable::options_sizing_stretch_same_width)
			.draw();


		//- TODO: it seems that the context menu only has to be one level above to work as expected.
		m_context_menu->on_ui_render();

		//const auto& entities = world_manager.active().em().entities();

		ImGui::BeginChild(C_BOTTOM_PANEL_ID.data(), {0.0f, 0.0f}, C_WORLD_INSPECTOR_CHILD_FLAGS, C_WORLD_INSPECTOR_FLAGS);

		show_menubar();

		switch (m_state.m_view_type)
		{
		default:
		case world_view_type_hierarchy:
		{
			show_view_hierarchy();
			break;
		}
		case world_view_type_layered:
		{
			show_view_layered();
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
			if (ImGui::BeginMenu("New"))
			{
				if (ImGui::MenuItem("Empty Entity"))
				{
					create_entity_default();

					state().m_dirty = true;
				}
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld_inspector::show_view_layered()
	{
		//- TODO: use cashing and recreate layered view only if something happened like a new layer added or removed
		//- TODO: how and where do we query for the currently created layers? sm does not care about it, so it should be our
		//- responsibility to create new ones and name them

// 		world.each([&](flecs::entity e, const ecs::ssprite& sprite, const ecs::sidentifier& identifier)
// 			{
// 				state().m_layered_view[sprite.m_layer].push_back(e);
// 			});
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld_inspector::show_view_hierarchy()
	{
		for (const auto& uuid : state().m_hierarchy_view.m_view)
		{
			const auto& e = state().m_snapshot.at(uuid);

			show_entity_hierarchy(e);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cworld_inspector::show_entity_ui(const ssnapshot_entity& e, unsigned depth)
	{
		if (ImGui::CollapsingHeader(e.m_name.data()))
		{
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld_inspector::show_entity_hierarchy(const ssnapshot_entity& e)
	{
		if (show_entity_ui(e, 0))
		{
			for (const auto& uuid : e.m_children)
			{
				const auto& kid = state().m_snapshot.at(uuid);

				show_entity_hierarchy_recursive(kid, 1);
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld_inspector::show_entity_hierarchy_recursive(const ssnapshot_entity& e, unsigned depth)
	{
		if (show_entity_ui(e, depth))
		{
			for (const auto& uuid : e.m_children)
			{
				const auto& kid = state().m_snapshot.at(uuid);

				show_entity_hierarchy_recursive(kid, depth + 1);
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld_inspector::create_entity_default()
	{
		ecs::cworld_manager::instance().active().em().create_entity()
			.add<ecs::stransform>()
			.add<ecs::sidentifier>()
			.add<ecs::shierarchy>()
			;

		state().m_dirty = true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld_inspector::recreate_snapshot()
	{
		CORE_NAMED_ZONE("cworld_inspector::recreate_snapshot");

		state().m_snapshot.clear();
		state().m_hierarchy_view.m_view.clear();
		state().m_layered_view.m_view.clear();

		ecs::cworld_manager::instance().active().world().each([&](flecs::entity e, const ecs::shierarchy& hierarchy, const ecs::sidentifier& identifier)
			{
				store_in_snapshot(e, hierarchy, identifier);
			});

		state().m_dirty = false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cworld_inspector::is_in_snapshot(const core::cuuid& uuid)
	{
		return state().m_snapshot.find(uuid) != state().m_snapshot.end();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld_inspector::store_in_snapshot(flecs::entity e, const ecs::shierarchy& hierarchy, const ecs::sidentifier& identifier)
	{
		//- Create snapshot entity from actual entity
		ssnapshot_entity snap;
		snap.m_uuid = identifier.m_uuid;
		snap.m_name = identifier.m_name;
		snap.m_entity = e;

		//- Check whether entity has children and/or parent
		snap.m_parent = hierarchy.m_parent;
		for (const auto& c : hierarchy.m_children)
		{
			snap.m_children.push_back(c);
		}

		//- Store in hierarchy view and/or layered view
		if (e.has<ecs::ssprite_renderer>())
		{
			const auto& sprite = e.get<ecs::ssprite_renderer>();

			state().m_layered_view.m_view[sprite->m_layer].push_back(snap.m_uuid);
		}

		if (snap.m_parent == core::cuuid::C_INVALID_UUID)
		{
			state().m_hierarchy_view.m_view.push_back(snap.m_uuid);
		}

		//- Store entity in snapshot
		state().m_snapshot[snap.m_uuid] = std::move(snap);
	}

} //- editor