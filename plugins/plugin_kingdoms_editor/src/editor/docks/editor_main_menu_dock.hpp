#pragma once
#include "editor_dock_base.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	class cmain_menu final : public clayer_base
	{
	public:
		cmain_menu(ccontext& ctx) : clayer_base(ctx) {};
		~cmain_menu() = default;

		bool init() override;
		void shutdown() override;
		void on_ui_render() override;

	private:

	};

} //- editor