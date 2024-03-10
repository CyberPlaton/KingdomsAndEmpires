#pragma once
#include "../../context/editor_context.hpp"

namespace editor
{
	//- TODO: refactor. This approach is not good and does not really work out as desired.
	//------------------------------------------------------------------------------------------------------------------------
	class centity_context_menu : public ccontext_holder
	{
	public:
		centity_context_menu(flecs::entity e, ccontext& ctx);
		~centity_context_menu();

		void on_ui_render();

	private:
		flecs::entity m_entity;
	};

} //- editor