#pragma once
#include "../editor_dock_base.hpp"
#include "../entity_context_menu/entity_context_menu.hpp"

namespace editor
{
	//- Window responsible for showing current snapshot of the edited world. From here yo can create new entities and
	//- select them for editing.
	//------------------------------------------------------------------------------------------------------------------------
	class EDITOR_API cworld_inspector final : public clayer_base
	{
	public:
		cworld_inspector(scontext& ctx) : clayer_base(ctx) {};
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

		struct sentity
		{
			core::cuuid m_uuid;
			std::string_view m_name;
			flecs::entity m_entity;
			//- hashed uuids of parent and children
			unsigned m_parent = MAX(unsigned);
			vector_t<unsigned> m_children;
		};

		struct sstate
		{
			vector_t<sentity> m_snapshot;
			umap_t<unsigned, unsigned> m_lookup;

			//- TODO: contains array of indices into snapshot. Each entity in here is
			//- guaranteed to have the 3 essential components: transform, identifier and hierarchy.
			map_t<unsigned, vector_t<unsigned>> m_layered_view;
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