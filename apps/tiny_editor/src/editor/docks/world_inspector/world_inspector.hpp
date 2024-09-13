#pragma once
#include "../editor_dock_base.hpp"
#include "../entity_context_menu/entity_context_menu.hpp"

namespace editor
{
	//- Window responsible for showing current snapshot of the edited world. From here yo can create new entities and
	//- select them for editing.
	//------------------------------------------------------------------------------------------------------------------------
	class cworld_inspector final : public clayer_base
	{
	public:
		cworld_inspector(scontext& ctx) : clayer_base("##world_inspector", ctx) {};
		~cworld_inspector() = default;

		bool init() override;
		void shutdown() override;
		void on_ui_render() override;

	private:
		using entity_id_t = flecs::id_t;

		enum world_view_type
		{
			world_view_type_none = 0,
			world_view_type_hierarchy,
			world_view_type_layered,
		};

		struct ssnapshot_entity
		{
			core::cuuid m_uuid;
			string_t m_name;
			vector_t<entity_id_t> m_children;
			flecs::entity m_entity;
			entity_id_t m_parent = MAX(entity_id_t);
		};

		struct slayered_view
		{
			map_t<unsigned, vector_t<unsigned>> m_view;
		};

		struct shierarchy_view
		{
			vector_t<unsigned> m_view;
		};

		struct sstate
		{
			umap_t<entity_id_t, unsigned> m_lookup;
			vector_t<ssnapshot_entity> m_snapshot;

			shierarchy_view m_hierarchy_view;
			slayered_view m_layered_view;

			world_view_type m_view_type = world_view_type_hierarchy;
			bool m_dirty = true;
		};

		sstate m_state;
		ref_t<centity_context_menu> m_context_menu;

	private:
		sstate& state() {return m_state;}
		void show_menubar();
		void show_rendering_layer_view(const flecs::world& world);
		void show_world_hierarchy_view(const flecs::world& world);
		void show_entity(const sentity& e, unsigned depth = 0);
		void create_default_entity();

		const sentity& get(unsigned hash);
		void recreate_snapshot(const flecs::world& world);
		bool present_in_snapshot(const core::cuuid& uuid);
		void emplace_in_snapshot(flecs::entity e, const ecs::shierarchy& hierarchy, const ecs::sidentifier& identifier);
	};

} //- editor