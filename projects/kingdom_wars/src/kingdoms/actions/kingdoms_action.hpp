#pragma once
#include <engine.h>
#include "../../kingdoms_game_context.hpp"

namespace kingdoms
{
	class iaction;
	using action_ref_t = ref_t<iaction>;

	//- Abstract action class. Encapsulates an action performed by an object in the game world. E.g.
	//- an elf shooting an arrow at an enemy is an 'Attack Action'.
	//------------------------------------------------------------------------------------------------------------------------
	class iaction
	{
	public:
		virtual ~iaction() = default;
		virtual void do_execute(const sgame_context&, float) = 0;

	private:

		RTTR_ENABLE();
	};

} //- kingdoms