#pragma once
#include "../../context/editor_context.hpp"

namespace editor
{
	//- TODO: refactor. This approach is not good and does not really work out as desired.
	//- Maybe we want derive this from some basic ui window class.
	//------------------------------------------------------------------------------------------------------------------------
	class centity_context_menu : public ccontext_holder
	{
	public:
		centity_context_menu(scontext& ctx);
		~centity_context_menu();

		void on_ui_render();
		void open(flecs::entity e);

	private:
		bool m_open = false;
		flecs::entity m_entity;
	};

} //- editor