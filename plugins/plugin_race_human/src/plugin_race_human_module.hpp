#pragma once
#include <plugin_kingdoms_core.h>

namespace races
{
	constexpr stringview_t C_HUMAN_CITIZEN_PREFAB_NAME = "prefab_human_citizen";


	//------------------------------------------------------------------------------------------------------------------------
	struct shuman : kingdoms::irace
	{
		DECLARE_COMPONENT(shuman);

		RTTR_ENABLE(kingdoms::irace);
	};

	//------------------------------------------------------------------------------------------------------------------------
	class chuman_race_module : public ecs::imodule
	{
	public:
		chuman_race_module(flecs::world& w) : ecs::imodule(w)
		{
			begin<chuman_race_module>("chuman_race_module")
				.comp<chuman_race_module, shuman>()
				.end<chuman_race_module>();

			shuman race_def;

			//- major attributes
			race_def.m_attributes.m_agility = 10;
			race_def.m_attributes.m_strength = 10;
			race_def.m_attributes.m_endurance = 10;

			//- minor attributes
			race_def.m_attributes.m_speed = 5;
			race_def.m_attributes.m_personality = 5;
			race_def.m_attributes.m_intelligence = 5;
			race_def.m_attributes.m_willpower = 5;
			race_def.m_attributes.m_luck = 5;

			//- major skills
			race_def.m_skills.m_alchemy = 25;
			race_def.m_skills.m_heavy_armor = 25;
			race_def.m_skills.m_medium_armor = 25;
			race_def.m_skills.m_light_armor = 25;
			race_def.m_skills.m_unarmored = 25;

			//- minor skills
			race_def.m_skills.m_spear = 15;
			race_def.m_skills.m_axe = 15;
			race_def.m_skills.m_blunt_weapon = 15;
			race_def.m_skills.m_long_blade = 15;
			race_def.m_skills.m_short_blade = 15;
			race_def.m_skills.m_athletics = 15;
			race_def.m_skills.m_block = 15;
			race_def.m_skills.m_marksman = 15;
			race_def.m_skills.m_acrobatics = 15;
			race_def.m_skills.m_sneak = 15;
			race_def.m_skills.m_mercantile = 15;
			race_def.m_skills.m_speechcraft = 15;
			race_def.m_skills.m_craftsmanship = 15;
			race_def.m_skills.m_illusion = 15;
			race_def.m_skills.m_alteration = 15;
			race_def.m_skills.m_conjuration = 15;
			race_def.m_skills.m_destruction = 15;
			race_def.m_skills.m_enchantment = 15;
			race_def.m_skills.m_mysticism = 15;
			race_def.m_skills.m_restoration = 15;

			//- derived attributes
			race_def.m_derived_attributes.m_health = 20;
			race_def.m_derived_attributes.m_action_points = 15;
			race_def.m_derived_attributes.m_mana = 10;

			//- define module prefabs
			auto human_citizen = w.prefab(C_HUMAN_CITIZEN_PREFAB_NAME.data())
						.emplace_override<shuman>(race_def)
						.emplace_override<kingdoms::iunit>()
						.add<professions::scitizen>()
						;
		}

		RTTR_ENABLE(ecs::imodule);
	};

} //- races

namespace races
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

} //- races