#pragma once
#include "editor_dock_base.hpp"
#include "world_inspector/world_inspector.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	class cleft_panel final : public clayer_base
	{
	public:
		cleft_panel(ccontext& ctx) : clayer_base(ctx) {};
		~cleft_panel() = default;

		bool init() override;
		void shutdown() override;
		void on_ui_render() override;

	private:
		ref_t<cworld_inspector> m_inspector;
	};

} //- editor