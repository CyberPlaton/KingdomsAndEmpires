#pragma once
#include <plugin_kingdoms_core.h>

namespace effects
{
	//- This is a dummy effect to show how one would be defined.
	//------------------------------------------------------------------------------------------------------------------------
	struct sstatus_effect final
	{
		static constexpr std::string_view C_EFFECT_UPDATE_FUNC_NAME		= "on_update";
		static constexpr std::string_view C_EFFECT_ON_APPLY_FUNC_NAME	= "on_apply";
		static constexpr std::string_view C_EFFECT_ON_REMOVE_FUNC_NAME	= "on_remove";

		static void on_apply(flecs::entity) {}
		static bool on_update(flecs::entity) {}
		static void on_remove(flecs::entity) {}
	};

	//- Specifies an entity as eligible for applying an effect to.
	//------------------------------------------------------------------------------------------------------------------------
	struct saffectable : ecs::icomponent
	{
		DECLARE_COMPONENT(saffectable);

		struct seffect_functions
		{
			rttr::method m_update;
			rttr::method m_on_apply;
			rttr::method m_on_remove;
			bool m_entered = true;
		}; 

		vector_t<seffect_functions> m_effects;

		RTTR_ENABLE(ecs::icomponent);
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cstatus_effects_system : public ecs::csystem<saffectable>
	{
	public:
		cstatus_effects_system(flecs::world& w) :
			ecs::csystem<stargeting_component>
			(w, "Status Effect System")
		{
			run_on(ecs::system_running_phase_on_update);
			multithreaded();
			build([&](flecs::entity e, saffectable& affectable)
				{
					ZoneScopedN("Status Effect System");

					vector_t<unsigned> to_be_removed;

					for(auto i = 0u; i < affectable.size(); ++i)
					{
						auto& effect = affectable.m_effects[i];

						if(effect.m_entered)
						{
							effect.m_on_apply.invoke({}, e);
							effect.m_entered = false;
						}

						if(!effect.m_on_update.invoke({}, e))
						{
							to_be_removed.push_back(i);
						}
					}

					for(auto i = 0u; i < to_be_removed.size(); ++i)
					{
						auto& effect = affectable.m_effects[to_be_removed[i]];

						effect.m_on_remove.invoke({}, e);

						algorithm::erase_at_index(affectable.m_effects, to_be_removed[i]);
					}
				});
		}
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cstatus_effects_module : public ecs::imodule
	{
	public:
		cstatus_effects_module(flecs::world& w) : ecs::imodule(w)
		{
			begin<cstatus_effects_module>("cstatus_effects_module")
				.comp<cstatus_effects_module, saffectable>()
				.subsystem<cstatus_effects_module, cstatus_effects_system>()
				.end<cstatus_effects_module>();
		}

		RTTR_ENABLE(ecs::imodule);
	};

} //- effects

namespace effects
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(shuman)
	{
		rttr::registration::class_<shuman>("shuman")
			.method("serialize", &shuman::serialize)
			;

		rttr::default_constructor<shuman>();
	};

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(chuman_race_module)
	{
		rttr::registration::class_<chuman_race_module>("chuman_race_module")
			.constructor<flecs::world&>()
			(
				rttr::policy::ctor::as_raw_ptr
			)
			;
	}

} //- effects