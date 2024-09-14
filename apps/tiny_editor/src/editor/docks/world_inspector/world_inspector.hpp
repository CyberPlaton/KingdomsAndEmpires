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
			vector_t<core::cuuid> m_children;
			flecs::entity m_entity;
			core::cuuid m_parent;
		};

		struct slayered_view
		{
			map_t<unsigned, vector_t<core::cuuid>> m_view;
		};

		struct shierarchy_view
		{
			vector_t<core::cuuid> m_view;
		};

		struct sstate
		{
			umap_t<core::cuuid, ssnapshot_entity> m_snapshot;

			shierarchy_view m_hierarchy_view;
			slayered_view m_layered_view;

			world_view_type m_view_type = world_view_type_hierarchy;
			bool m_dirty = true;
		};

		sstate m_state;
		ref_t<centity_context_menu> m_context_menu;

	private:
		inline sstate& state() { return m_state; }
		inline const sstate& state() const { return m_state; }
		void show_menubar();
		void show_view_layered();
		void show_view_hierarchy();

		bool show_entity_ui(const ssnapshot_entity& e, unsigned depth);
		void show_entity_hierarchy(const ssnapshot_entity& e);
		void show_entity_hierarchy_recursive(const ssnapshot_entity& e, unsigned depth);

		void create_entity_default();
		void recreate_snapshot();
		bool is_in_snapshot(const core::cuuid& uuid);
		void store_in_snapshot(flecs::entity e, const ecs::shierarchy& hierarchy, const ecs::sidentifier& identifier);
	};

} //- editor