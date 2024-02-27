#pragma once
#include "editor_dock_base.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	class cbottom_panel final : public clayer_base
	{
	public:
		cbottom_panel() = default;
		~cbottom_panel() = default;

		bool init() override;
		void shutdown() override;
		void on_ui_render() override;

	private:

	};

} //- editor