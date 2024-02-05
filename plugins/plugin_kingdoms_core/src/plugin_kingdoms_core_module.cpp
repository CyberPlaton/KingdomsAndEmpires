#include "plugin_kingdoms_core_module.hpp"

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace kingdoms;
	using namespace technology;

	registration::enumeration<kingdom_race>("kingdom_race")
		(
			value("kingdom_race_none",		kingdom_race::kingdom_race_none),
			value("kingdom_race_human",		kingdom_race::kingdom_race_human),
			value("kingdom_race_orc",		kingdom_race::kingdom_race_orc),
			value("kingdom_race_goblin",	kingdom_race::kingdom_race_goblin),
			value("kingdom_race_elf",		kingdom_race::kingdom_race_elf),
			value("kingdom_race_gnome",		kingdom_race::kingdom_race_gnome),
			value("kingdom_race_dwarf",		kingdom_race::kingdom_race_dwarf),
			value("kingdom_race_troll",		kingdom_race::kingdom_race_high_troll),
			value("kingdom_race_dark_elf",	kingdom_race::kingdom_race_dark_elf))
		;

	registration::class_<itech>("itech")
		.property("m_name",				&itech::m_name)
		.property("m_requirements",		&itech::m_requirements)
		.property("m_parent",			&itech::m_parent)
		.property("m_race_requirement",	&itech::m_race_requirement)
		.property("m_points",			&itech::m_points)
		.property("m_category",			&itech::m_category)
		.property("m_callback",			&itech::m_callback)
		;

	registration::class_<sattributes>("sattributes")
		.property("m_strength",		&sattributes::m_strength)
		.property("m_endurance",	&sattributes::m_endurance)
		.property("m_agility",		&sattributes::m_agility)
		.property("m_speed",		&sattributes::m_speed)
		.property("m_personality",	&sattributes::m_personality)
		.property("m_intelligence",	&sattributes::m_intelligence)
		.property("m_willpower",	&sattributes::m_willpower)
		.property("m_luck",			&sattributes::m_luck)
		;

	registration::class_<sderived_attributes>("sderived_attributes")
		.property("m_health",		&sderived_attributes::m_health)
		.property("m_mana",			&sderived_attributes::m_mana)
		.property("m_fatigue",		&sderived_attributes::m_fatigue)
		;

	registration::class_<sskills>("sskills")
		.property("m_heavy_armor",	&sskills::m_heavy_armor)
		.property("m_medium_armor",	&sskills::m_medium_armor)
		.property("m_light_armor",	&sskills::m_light_armor)
		.property("m_unarmored",	&sskills::m_unarmored)
		.property("m_spear",		&sskills::m_spear)
		.property("m_axe",			&sskills::m_axe)
		.property("m_blunt_weapon",	&sskills::m_blunt_weapon)
		.property("m_long_blade",	&sskills::m_long_blade)
		.property("m_short_blade",	&sskills::m_short_blade)
		.property("m_athletics",	&sskills::m_athletics)
		.property("m_block",		&sskills::m_block)
		.property("m_marksman",		&sskills::m_marksman)
		.property("m_acrobatics",	&sskills::m_acrobatics)
		.property("m_sneak",		&sskills::m_sneak)
		.property("m_mercantile",	&sskills::m_mercantile)
		.property("m_speechcraft",	&sskills::m_speechcraft)
		.property("m_craftsmanship",&sskills::m_craftsmanship)
		.property("m_illusion",		&sskills::m_illusion)
		.property("m_alchemy",		&sskills::m_alchemy)
		.property("m_alteration",	&sskills::m_alteration)
		.property("m_conjuration",	&sskills::m_conjuration)
		.property("m_destruction",	&sskills::m_destruction)
		.property("m_enchantment",	&sskills::m_enchantment)
		.property("m_mysticism",	&sskills::m_mysticism)
		.property("m_restoration",	&sskills::m_restoration)
		;

};

namespace technology
{
	//------------------------------------------------------------------------------------------------------------------------
	bool available_for_race(const ref_t<itech>& tech, kingdoms::kingdom_race race)
	{
		return algorithm::bit_on(tech->m_race_requirement, race);
	}

} //- technology