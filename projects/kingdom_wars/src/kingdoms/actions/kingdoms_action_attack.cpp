#include "kingdoms_action_attack.hpp"

namespace kingdoms
{
	//------------------------------------------------------------------------------------------------------------------------
	void iattack::do_execute(const sgame_context& ctx, float dt)
	{
		const auto& w = ecs::cworld_manager::instance().active();
		const auto selected_entity = w.em().entity(ctx.m_selected_entity);
		const auto target_entity = w.em().entity(ctx.m_target_entity);

		if (selected_entity.is_valid() && target_entity.is_valid() &&
			can_attack(w, selected_entity, target_entity, dt))
		{
			do_attack(w, selected_entity, target_entity, dt);
		}
	}

} //- kingdoms