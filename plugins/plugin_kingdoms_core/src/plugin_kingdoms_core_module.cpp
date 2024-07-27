#include "plugin_kingdoms_core_module.hpp"

namespace technology
{
	//------------------------------------------------------------------------------------------------------------------------
	bool available_for_race(const ref_t<itech>& tech, kingdoms::kingdom_race race)
	{
		return algorithm::bit_check(tech->m_race_requirement, race);
	}

} //- technology

namespace settlement
{
	//------------------------------------------------------------------------------------------------------------------------
	bool can_settlement_produce_resource(const isettlement& city, const umap_t<std::string, unsigned>& input)
	{
		const auto& resources = city.m_data.m_resources;

		for (const auto& inpair : input)
		{
			auto it = resources.find(inpair.first);
			if (it == resources.end())
			{
				//- required resource not present
				return false;
			}
			else if (it->second < inpair.second)
			{
				//- not enough of required resource present
				return false;
			}
		}
		return true;
	}

} //- settlement

RTTR_REGISTRATION
{
	using namespace kingdoms;

rttr::registration::enumeration<kingdom_race>("kingdom_race")
(
	rttr::value("kingdom_race_none", kingdom_race::kingdom_race_none),
	rttr::value("kingdom_race_human", kingdom_race::kingdom_race_human),
	rttr::value("kingdom_race_orc", kingdom_race::kingdom_race_orc),
	rttr::value("kingdom_race_goblin", kingdom_race::kingdom_race_goblin),
	rttr::value("kingdom_race_elf", kingdom_race::kingdom_race_elf),
	rttr::value("kingdom_race_gnome", kingdom_race::kingdom_race_gnome),
	rttr::value("kingdom_race_dwarf", kingdom_race::kingdom_race_dwarf),
	rttr::value("kingdom_race_troll", kingdom_race::kingdom_race_high_troll),
	rttr::value("kingdom_race_dark_elf", kingdom_race::kingdom_race_dark_elf)
	)
	;

rttr::registration::class_<sattributes>("sattributes")
.constructor<>()
(
	rttr::policy::ctor::as_object
	)
	.property("m_strength", &sattributes::m_strength)
	.property("m_endurance", &sattributes::m_endurance)
	.property("m_agility", &sattributes::m_agility)
	.property("m_speed", &sattributes::m_speed)
	.property("m_personality", &sattributes::m_personality)
	.property("m_intelligence", &sattributes::m_intelligence)
	.property("m_willpower", &sattributes::m_willpower)
	.property("m_luck", &sattributes::m_luck)
	;

rttr::registration::class_<sderived_attributes>("sderived_attributes")
.constructor<>()
(
	rttr::policy::ctor::as_object
	)
	.property("m_health", &sderived_attributes::m_health)
	.property("m_mana", &sderived_attributes::m_mana)
	.property("m_action_points", &sderived_attributes::m_action_points)
	;

rttr::registration::class_<sskills>("sskills")
.constructor<>()
(
	rttr::policy::ctor::as_object
	)
	(
		metadata(C_DISPLAY_NAME_PROP, "Skills")
		)
	.property("m_heavy_armor", &sskills::m_heavy_armor)
	(
		metadata(C_DISPLAY_NAME_PROP, "Heavy Armor")
		)
	.property("m_medium_armor", &sskills::m_medium_armor)
	(
		metadata(C_DISPLAY_NAME_PROP, "Medium Armor")
		)
	.property("m_light_armor", &sskills::m_light_armor)
	(
		metadata(C_DISPLAY_NAME_PROP, "Light Armor")
		)
	.property("m_unarmored", &sskills::m_unarmored)
	(
		metadata(C_DISPLAY_NAME_PROP, "Unarmored")
		)
	.property("m_spear", &sskills::m_spear)
	(
		metadata(C_DISPLAY_NAME_PROP, "Spear")
		)
	.property("m_axe", &sskills::m_axe)
	(
		metadata(C_DISPLAY_NAME_PROP, "Axe")
		)
	.property("m_blunt_weapon", &sskills::m_blunt_weapon)
	(
		metadata(C_DISPLAY_NAME_PROP, "Blunt Weapon")
		)
	.property("m_long_blade", &sskills::m_long_blade)
	(
		metadata(C_DISPLAY_NAME_PROP, "Long Blade")
		)
	.property("m_short_blade", &sskills::m_short_blade)
	(
		metadata(C_DISPLAY_NAME_PROP, "Short Blade")
		)
	.property("m_athletics", &sskills::m_athletics)
	(
		metadata(C_DISPLAY_NAME_PROP, "Athletics")
		)
	.property("m_block", &sskills::m_block)
	(
		metadata(C_DISPLAY_NAME_PROP, "Block")
		)
	.property("m_marksman", &sskills::m_marksman)
	(
		metadata(C_DISPLAY_NAME_PROP, "Marksman")
		)
	.property("m_acrobatics", &sskills::m_acrobatics)
	(
		metadata(C_DISPLAY_NAME_PROP, "Acrobatics")
		)
	.property("m_sneak", &sskills::m_sneak)
	(
		metadata(C_DISPLAY_NAME_PROP, "Sneak")
		)
	.property("m_mercantile", &sskills::m_mercantile)
	(
		metadata(C_DISPLAY_NAME_PROP, "Mercantile")
		)
	.property("m_speechcraft", &sskills::m_speechcraft)
	(
		metadata(C_DISPLAY_NAME_PROP, "Speechcraft")
		)
	.property("m_craftsmanship", &sskills::m_craftsmanship)
	(
		metadata(C_DISPLAY_NAME_PROP, "Craftsmanship")
		)
	.property("m_illusion", &sskills::m_illusion)
	(
		metadata(C_DISPLAY_NAME_PROP, "Illusion")
		)
	.property("m_alchemy", &sskills::m_alchemy)
	(
		metadata(C_DISPLAY_NAME_PROP, "Alchemy")
		)
	.property("m_alteration", &sskills::m_alteration)
	(
		metadata(C_DISPLAY_NAME_PROP, "Alteration")
		)
	.property("m_conjuration", &sskills::m_conjuration)
	(
		metadata(C_DISPLAY_NAME_PROP, "Conjuration")
		)
	.property("m_destruction", &sskills::m_destruction)
	(
		metadata(C_DISPLAY_NAME_PROP, "Destruction")
		)
	.property("m_enchantment", &sskills::m_enchantment)
	(
		metadata(C_DISPLAY_NAME_PROP, "Enchantment")
		)
	.property("m_mysticism", &sskills::m_mysticism)
	(
		metadata(C_DISPLAY_NAME_PROP, "Mysticism")
		)
	.property("m_restoration", &sskills::m_restoration)
	(
		metadata(C_DISPLAY_NAME_PROP, "Restoration")
		)
	;

		rttr::registration::class_<irace>("irace")
			.constructor<>()
			(
				rttr::policy::ctor::as_object
				)
			.property("m_attributes", &irace::m_attributes)
			.property("m_skills", &irace::m_skills)
			.property("m_derived_attributes", &irace::m_derived_attributes)
			;


	using namespace technology;

	rttr::registration::enumeration<tech_category>("tech_category")
		(
			rttr::value("tech_category_none", tech_category::tech_category_none),
			rttr::value("tech_category_technology", tech_category::tech_category_technology),
			rttr::value("tech_category_culture", tech_category::tech_category_culture),
			rttr::value("tech_category_magick", tech_category::tech_category_magick)
			);

	rttr::registration::class_<itech>("itech")
		.constructor<>()
		(
			rttr::policy::ctor::as_object
			)
		.property("m_name", &itech::m_name)
		.property("m_requirements", &itech::m_requirements)
		.property("m_parent", &itech::m_parent)
		.property("m_race_requirement", &itech::m_race_requirement)
		.property("m_points", &itech::m_points)
		.property("m_category", &itech::m_category)
		;

	using namespace professions;

	rttr::registration::class_<scitizen>("scitizen");
	rttr::registration::class_<swoodcutter>("swoodcutter");
	rttr::registration::class_<sspearman>("sspearman");

	using namespace resource;

	rttr::registration::class_<iresource>("iresource");
	rttr::registration::class_<swood>("swood");
	rttr::registration::class_<sclay>("sclay");
	rttr::registration::class_<sfood>("sfood");
	rttr::registration::class_<sleather>("sleather");

	using namespace building;
}