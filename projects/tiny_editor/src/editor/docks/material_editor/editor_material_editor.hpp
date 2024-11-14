#pragma once
#include "../editor_dock_base.hpp"
#include "material_graph.hpp"
#include "material_generator.hpp"
#include "material_nodes.hpp"

namespace editor
{
	//- Responsible for creating materials in a visual way.
	//------------------------------------------------------------------------------------------------------------------------
	class cmaterial_editor final : public clayer_base
	{
	public:
		cmaterial_editor(scontext& ctx);
		~cmaterial_editor();

		bool init() override final;
		void shutdown() override final;
		void on_update(float dt) override final;
		void on_ui_render() override final;
		void on_world_render() override final;

	private:
		material_graph_ref_t m_graph;
		ImNodes::EditorContext* m_editor;
	};

} //- editor