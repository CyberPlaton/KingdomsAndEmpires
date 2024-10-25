#include "plugin_race_human_module.hpp"

namespace races
{
	namespace
	{
		constexpr stringview_t C_HUMAN_CITIZEN_PREFAB_NAME = "prefab_human_citizen";

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	shuman_race_module::shuman_race_module(ecs::cworld* w)
	{
		ecs::modules::sconfig cfg;
		cfg.m_name = "Human Race Module";
		cfg.m_components = { "shuman" };

		w->import_module<shuman_race_module>(cfg);

		//- Note: move prefab definition to somewhere else
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
		auto human_citizen = w->ecs().prefab(C_HUMAN_CITIZEN_PREFAB_NAME.data())
			.emplace_auto_override<shuman>(race_def)
			.emplace_auto_override<kingdoms::iunit>()
			.add<professions::scitizen>()
			;
	}

} //- races

RTTR_REGISTRATION
{
	using namespace races;

	rttr::registration::class_<shuman>("shuman")
		.method("serialize", &shuman::serialize)
		;

	rttr::detail::default_constructor<shuman>();
}