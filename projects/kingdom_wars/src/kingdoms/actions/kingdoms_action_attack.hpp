#pragma once
#include "kingdoms_action.hpp"

namespace kingdoms
{
	//- Abstract interface for an object that performs an attack action.
	//------------------------------------------------------------------------------------------------------------------------
	class iattack : public iaction
	{
	public:
		virtual bool can_attack(const ecs::cworld& world, ecs::centity attacker, ecs::centity attacked, float dt) = 0;
		virtual void do_attack(const ecs::cworld& world, ecs::centity attacker, ecs::centity attacked, float dt) = 0;

		void do_execute(const sgame_context& ctx, float dt) override final;

	private:

		RTTR_ENABLE(iaction);
	};

} //- kingdoms