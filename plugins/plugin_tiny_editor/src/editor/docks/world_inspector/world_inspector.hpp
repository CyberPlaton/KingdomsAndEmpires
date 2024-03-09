#pragma once
#include "../editor_dock_base.hpp"

namespace editor
{
	//- Not a dock or layer per-se, intended to be used directly by the respective panel.
	//------------------------------------------------------------------------------------------------------------------------
	class cworld_inspector final : public clayer_base
	{
	public:
		cworld_inspector(ccontext& ctx) : clayer_base(ctx) {};
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
			map_t<renderlayer_t, vector_t<unsigned>> m_layered_view;
			world_view_type m_view_type = world_view_type_hierarchy;
			bool m_dirty = true;
		};

		sstate m_state;

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