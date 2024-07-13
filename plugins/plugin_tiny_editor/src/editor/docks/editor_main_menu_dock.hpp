#pragma once
#include "editor_dock_base.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	class cmain_menu final : public clayer_base
	{
	public:
		cmain_menu(scontext& ctx) : clayer_base(ctx) {};
		~cmain_menu() = default;

		bool init() override final;
		void shutdown() override final;
		void on_update(float dt) override final;
		void on_ui_render() override final;

	private:

	};

} //- editor