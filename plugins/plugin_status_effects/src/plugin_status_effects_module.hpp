#pragma once
#include <plugin_kingdoms_core.h>

namespace effects
{
	//- This is a dummy effect to show how one would be defined.
	//- You would implement desired functions. Where on_apply is called once when the effect is applied and
	//- the turn tick start, on_update is invoked on each turn tick and return type false indicates that it should be removed,
	//- on_remove is called just before the effect is removed.
	//------------------------------------------------------------------------------------------------------------------------
	struct sstatus_effect final
	{
		static constexpr std::string_view C_EFFECT_ON_UPDATE_FUNC_NAME	= "on_update";
		static constexpr std::string_view C_EFFECT_ON_APPLY_FUNC_NAME	= "on_apply";
		static constexpr std::string_view C_EFFECT_ON_REMOVE_FUNC_NAME	= "on_remove";

		void on_apply(flecs::entity) {}
		bool on_update(flecs::entity) {return false;}
		void on_remove(flecs::entity) {}

		RTTR_ENABLE();
	};

	//- Specifies an entity as eligible for applying an effect to.
	//- m_effect is a status effect class instance that ought to be serializable and reflected.
	//------------------------------------------------------------------------------------------------------------------------
	struct saffectable : ecs::icomponent
	{
		DECLARE_COMPONENT(saffectable);

		struct sdata
		{
			rttr::variant m_effect;
			bool m_entered = true;
		}; 

		vector_t<sdata> m_active_effects;

		RTTR_ENABLE(ecs::icomponent);
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cstatus_effects_system : public ecs::csystem<saffectable>
	{
	public:
		cstatus_effects_system(flecs::world& w) :
			ecs::csystem<saffectable>
			(w, "Status Effect System")
		{
			run_on(ecs::system_running_phase_on_update);
			multithreaded();
			build([&](flecs::entity e, saffectable& affectable)
				{
					ZoneScopedN("Status Effect System");

					vector_t<unsigned> to_be_removed;

					for (auto i = 0u; i < affectable.m_active_effects.size(); ++i)
					{
						auto& effect = affectable.m_active_effects[i];

						auto type = effect.m_effect.get_type();

						//- enter newly applied effects
						if (effect.m_entered)
						{
							//- find required method
							if (auto apply = type.get_method(sstatus_effect::C_EFFECT_ON_APPLY_FUNC_NAME.data()); apply.is_valid())
							{
								apply.invoke(effect.m_effect, e);
							}
							effect.m_entered = false;
						}

						//- regular effects update
						if (auto update = type.get_method(sstatus_effect::C_EFFECT_ON_UPDATE_FUNC_NAME.data()); update.is_valid())
						{
							if (auto var = update.invoke(effect.m_effect, e); !var.to_bool())
							{
								to_be_removed.push_back(i);
							}
						}
					}

					//- remove effects that are completed
					for (auto i : to_be_removed)
					{
						auto& effect = affectable.m_active_effects[to_be_removed[i]];

						auto type = effect.m_effect.get_type();

						if (auto remove = type.get_method(sstatus_effect::C_EFFECT_ON_REMOVE_FUNC_NAME.data()); remove.is_valid())
						{
							remove.invoke(effect.m_effect, e);
						}

						algorithm::erase_at_index(affectable.m_active_effects, to_be_removed[i]);
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
	REFLECT_INLINE(sstatus_effect)
	{
		rttr::registration::class_<sstatus_effect>("sstatus_effect")
			.method("on_apply", &sstatus_effect::on_apply)
			.method("on_update", &sstatus_effect::on_update)
			.method("on_remove", &sstatus_effect::on_remove)
			;

		rttr::default_constructor<sstatus_effect>();
	};

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(saffectable)
	{
		rttr::registration::class_<saffectable>("saffectable")
			.property("m_active_effects", &saffectable::m_active_effects)
			;

		rttr::default_constructor<saffectable>();
	}

} //- effects