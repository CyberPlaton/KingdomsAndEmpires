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

		struct sstate
		{
			map_t<unsigned char, vector_t<flecs::entity>> m_layered_view;
			world_view_type m_view_type = world_view_type_hierarchy;
		};

		sstate m_state;

	private:
		void show_menubar();
		void show_rendering_layer_view(const flecs::world& world);
		void show_world_hierarchy_view(const flecs::world& world);
	};

} //- editor