#include "world_inspector.hpp"

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
			if (ImGui::BeginMenu("New"))
			{
				if (ImGui::MenuItem("Entity"))
				{
					create_default_entity();

					state().m_dirty = true;
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

		state().m_layered_view.clear();

		world.each([&](flecs::entity e, const ecs::ssprite& sprite, const ecs::sidentifier& identifier)
			{
				state().m_layered_view[sprite.m_layer].push_back(e);
			});
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld_inspector::show_world_hierarchy_view(const flecs::world& world)
	{
		//- TODO: use caching and recreate hierarchy view only if something happened
		if (state().m_dirty)
		{
			recreate_snapshot(world);
		}

		for (const auto& e : state().m_snapshot)
		{
			show_entity(e);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld_inspector::show_entity(const sentity& e, unsigned depth /*= 0*/)
	{
		ui::scope::cindent indent_scope(depth);

		//- TODO: implement tree node class like the button, so we can detect double-clicks etc
		//- and customize its appearance

		auto flags = C_TREE_NODE_BASE_FLAGS;

		if (e.m_children.empty())
		{
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet;
		}


		//- show entity
		ui::ctreenode node;
		auto result = node.title(e.m_uuid.string().c_str())
			.tooltip("A hint")
			.icon(ICON_FA_ANKH)
			.flags(flags).show();

		if (result == ui::detail::icontrol::click_result_lmb)
		{
			sm::cui::create_notification("Notification", "LMB", sm::notification_type_info);

			//- recursively show children
			for (const auto& k : e.m_children)
			{
				const auto& kid = get(k);

				show_entity(kid, depth + 1);
			}
		}
		else if (result == ui::detail::icontrol::click_result_rmb)
		{
			sm::cui::create_notification("Notification", "RMB", sm::notification_type_info);

			ctx().m_inspected_entity = e.m_uuid;
		}
		else if (result == ui::detail::icontrol::click_result_lmb_repeated)
		{
			sm::cui::create_notification("Notification", "LMB repeated", sm::notification_type_info);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld_inspector::create_default_entity()
	{
		ecs::cworld_manager::instance().active().em().create_entity()
			.add<ecs::stransform>()
			.add<ecs::sidentifier>()
			.add<ecs::shierarchy>()
			;
	}

	//------------------------------------------------------------------------------------------------------------------------
	const editor::cworld_inspector::sentity& cworld_inspector::get(unsigned hash)
	{
		return state().m_snapshot[state().m_lookup.at(hash)];
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld_inspector::recreate_snapshot(const flecs::world& world)
	{
		{
			CORE_NAMED_ZONE("World Inspector recreate snapshot");

			state().m_snapshot.clear();
			state().m_lookup.clear();

			world.each([&](flecs::entity e, const ecs::shierarchy& hierarchy, const ecs::sidentifier& identifier)
				{
					if (!present_in_snapshot(identifier.m_uuid))
					{
						emplace_in_snapshot(e, hierarchy, identifier);
					}
				});

			state().m_dirty = false;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cworld_inspector::present_in_snapshot(const core::cuuid& uuid)
	{
		auto h = uuid.hash();

		return algorithm::find_if(state().m_lookup.begin(), state().m_lookup.end(),
			[=](const auto& pair)
			{
				return pair.first == h;

			}) != state().m_lookup.end();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld_inspector::emplace_in_snapshot(flecs::entity e, const ecs::shierarchy& hierarchy, const ecs::sidentifier& identifier)
	{
		//- create entity for snapshot
		sentity entity;
		entity.m_uuid = identifier.m_uuid;
		entity.m_name = identifier.m_name;
		entity.m_entity = e;
		entity.m_parent = identifier.m_uuid.hash();
		for (const auto& uuid : hierarchy.m_children)
		{
			entity.m_children.push_back(uuid.hash());
		}

		//- store entity in snapshot
		auto h = identifier.m_uuid.hash();
		auto index = state().m_snapshot.size();
		state().m_snapshot.emplace_back(std::move(entity));
		state().m_lookup[h] = index;
	}

} //- editor