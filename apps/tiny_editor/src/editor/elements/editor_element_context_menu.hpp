#pragma once
#include <core.h>
#include <engine.h>
#include "../docks/editor_dock_base.hpp"

namespace editor::ui
{
	//- An extendable context menu, or popup in imgui terms, that is opened and allows for some actions to occurr.
	//------------------------------------------------------------------------------------------------------------------------
	class ccontext_menu
	{
	public:
		ccontext_menu(const char* id, scontext& ctx, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
		~ccontext_menu();

		ccontext_menu& open(const bool value);
		ccontext_menu& flags(ImGuiWindowFlags flags);

		bool draw();

	private:
		scontext& m_ctx;
		const char* m_id;
		ImGuiWindowFlags m_flags;
	};

} //- editor::ui