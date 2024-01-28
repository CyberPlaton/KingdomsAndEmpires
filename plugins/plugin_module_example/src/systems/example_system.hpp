#pragma once
#include "../plugin_module_example_common.hpp"
#include "../components/example_components.hpp"

namespace module_example
{
	//- Example module system
	//------------------------------------------------------------------------------------------------------------------------
	class EXAMPLE_API cmy_system : public ecs::csystem
	{
	public:
		cmy_system(ref_t<flecs::world> w) :
			ecs::csystem(w)
		{
			subsystem([&](flecs::world& w) -> subsystem_registrator_return_t
				{
					static constexpr auto C_DT = 0.016f;

					auto sys = w.system<stargeting_component>("Targeting System")
						.each([](stargeting_component& target)
							{
								//- check for first start of the system or
								//- end of cooldown and restart
								if (target.m_cooldown == stargeting_component::C_TARGET_COOLDOWN_INVALID ||
									target.m_cooldown <= 0.0f)
								{
									target.m_next_target = core::cuuid();
									target.m_cooldown = stargeting_component::C_TARGET_COOLDOWN_TIMER;

									logging::log_info(fmt::format("[Targeting System] Changing Target '{}'", target.m_next_target.view()).data());
								}
								target.m_cooldown -= C_DT;
							});

					//- Return main system, without any dependencies
					return { sys, {} };
				});
		}
	};

} //- module_example