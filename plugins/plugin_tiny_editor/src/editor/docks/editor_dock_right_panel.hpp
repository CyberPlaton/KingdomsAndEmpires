#pragma once
#include "editor_dock_base.hpp"
#include "entity_inspector/entity_inspector.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	class cright_panel final : public clayer_base
	{
	public:
		cright_panel(ccontext& ctx) : clayer_base(ctx) {};
		~cright_panel() = default;

		bool init() override;
		void shutdown() override;
		void on_ui_render() override;

	private:
		ref_t<centity_inspector> m_inspector;
	};

} //- editor