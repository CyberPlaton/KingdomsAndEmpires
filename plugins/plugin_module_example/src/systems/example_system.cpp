#include "example_system.hpp"

namespace module_example
{
	void targeting_system(flecs::world& world)
	{
		static constexpr auto C_DT = 0.016f;

		world.system<component::stargeting>()
			.each([](component::stargeting& target)
				{
					//- check for first start of the system or
					//- end of cooldown and restart
					if (target.m_cooldown == component::stargeting::C_TARGET_COOLDOWN_INVALID ||
						target.m_cooldown <= 0.0f)
					{
						target.m_next_target = core::cuuid();
						target.m_cooldown = component::stargeting::C_TARGET_COOLDOWN_TIMER;

						logging::log_info(fmt::format("[targeting_system] Chaning Target '{}'", target.m_next_target.view()));
					}
					target.m_cooldown -= C_DT;
				});
	}

} //- module_example