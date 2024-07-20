#pragma once
#include "../editor_dock_base.hpp"
#include "../../nodes/editor_nodes_context.hpp"

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

	private:
		ImNodesEditorContext* m_ctx;
	};

} //- editor