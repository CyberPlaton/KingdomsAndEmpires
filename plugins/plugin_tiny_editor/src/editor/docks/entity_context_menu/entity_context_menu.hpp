#pragma once
#include "../../context/editor_context.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	class centity_context_menu : public ccontext_holder
	{
	public:
		centity_context_menu(flecs::entity e, ccontext& ctx);
		~centity_context_menu();

	private:
	};

} //- editor