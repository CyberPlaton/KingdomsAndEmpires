#pragma once
#include <core.h>
#include <engine.h>
#include <rttr.h>

namespace kingdoms
{
	constexpr rttr::string_view C_DISPLAY_NAME_PROP = "DisplayName";

	using kingdom_t = handle_type_t;
	using technology_t = handle_type_t;

	//------------------------------------------------------------------------------------------------------------------------
	enum kingdom_race : uint8_t
	{
		kingdom_race_none = 0,
		kingdom_race_human,
		kingdom_race_orc,
		kingdom_race_goblin,
		kingdom_race_elf,
		kingdom_race_gnome,
		kingdom_race_dwarf,
		kingdom_race_high_troll,
		kingdom_race_dark_elf,
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sattributes : ecs::icomponent
	{
		static constexpr unsigned C_ATTRIBUTE_VALUE_MAX = 100;

		DECLARE_COMPONENT(sattributes);

		unsigned m_strength = 0;
		unsigned m_endurance = 0;
		unsigned m_agility = 0;
		unsigned m_speed = 0;
		unsigned m_personality = 0;
		unsigned m_intelligence = 0;
		unsigned m_willpower = 0;
		unsigned m_luck = 0;

		RTTR_ENABLE(ecs::icomponent);
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sderived_attributes : ecs::icomponent
	{
		static constexpr unsigned C_DERIVED_ATTRIBUTE_VALUE_MAX = 100;

		DECLARE_COMPONENT(sderived_attributes);

		unsigned m_health = 0;
		unsigned m_mana = 0;
		unsigned m_fatigue = 0;

		RTTR_ENABLE(ecs::icomponent);
	};


	//------------------------------------------------------------------------------------------------------------------------
	struct sskills : ecs::icomponent
	{
		static constexpr unsigned C_SKILL_VALUE_MAX = 100;

		DECLARE_COMPONENT(sskills);

		unsigned m_heavy_armor = 0;
		unsigned m_medium_armor = 0;
		unsigned m_light_armor = 0;
		unsigned m_unarmored = 0;
		unsigned m_spear = 0;
		unsigned m_axe = 0;
		unsigned m_blunt_weapon = 0;
		unsigned m_long_blade = 0;
		unsigned m_short_blade = 0;
		unsigned m_athletics = 0;
		unsigned m_block = 0;
		unsigned m_marksman = 0;
		unsigned m_acrobatics = 0;
		unsigned m_sneak = 0;
		unsigned m_mercantile = 0;
		unsigned m_speechcraft = 0;
		unsigned m_craftsmanship = 0;
		unsigned m_illusion = 0;
		unsigned m_alchemy = 0;
		unsigned m_alteration = 0;
		unsigned m_conjuration = 0;
		unsigned m_destruction = 0;
		unsigned m_enchantment = 0;
		unsigned m_mysticism = 0;
		unsigned m_restoration = 0;

		RTTR_ENABLE(ecs::icomponent);
	};

} //- kingdoms

namespace technology
{
	using on_researched_callback_t = std::function<bool(kingdoms::kingdom_t)>;

	//- forward decl.
	struct itech;

	bool available_for_race(const ref_t<itech>& tech, kingdoms::kingdom_race race);

	//------------------------------------------------------------------------------------------------------------------------
	enum tech_category : uint8_t
	{
		tech_category_none = 0,
		tech_category_technology,
		tech_category_culture,
		tech_category_magick,
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct itech
	{
		string_t m_name;
		vector_t<ref_t<itech>> m_requirements;
		ref_t<itech> m_parent;
		int m_race_requirement;
		unsigned m_points;
		tech_category m_category;

		on_researched_callback_t m_callback;

		RTTR_ENABLE();
	};

} //- technology

namespace professions
{
	//- each kingdom unit starts life as a citizen, when choosing a profession the citizen component
	//- is replaced with the other component and the unit becomes i.e. a 'Spearman' and 'gains' new abilities
	//------------------------------------------------------------------------------------------------------------------------
	struct scitizen : ecs::icomponent
	{
		DECLARE_COMPONENT(scitizen);

		RTTR_ENABLE(ecs::icomponent);
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct swoodcutter : scitizen
	{
		DECLARE_COMPONENT(swoodcutter);

		RTTR_ENABLE(scitizen);
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sspearman : scitizen
	{
		DECLARE_COMPONENT(sspearman);

		RTTR_ENABLE(scitizen);
	};


} //- professions


namespace kingdoms
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(kingdom_race)
	{
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
	};

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(sattributes)
	{
		rttr::registration::class_<sattributes>("sattributes")
			.property("m_strength", &sattributes::m_strength)
			.property("m_endurance", &sattributes::m_endurance)
			.property("m_agility", &sattributes::m_agility)
			.property("m_speed", &sattributes::m_speed)
			.property("m_personality", &sattributes::m_personality)
			.property("m_intelligence", &sattributes::m_intelligence)
			.property("m_willpower", &sattributes::m_willpower)
			.property("m_luck", &sattributes::m_luck)
			;
	};

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(sderived_attributes)
	{
		rttr::registration::class_<sderived_attributes>("sderived_attributes")
			.property("m_health", &sderived_attributes::m_health)
			.property("m_mana", &sderived_attributes::m_mana)
			.property("m_fatigue", &sderived_attributes::m_fatigue)
			;
	};

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(sskills)
	{
		rttr::registration::class_<sskills>("sskills")
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
	};

} //- kingdoms

namespace technology
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(tech_category)
	{
		rttr::registration::enumeration<tech_category>("tech_category")
			(
				rttr::value("tech_category_none", tech_category::tech_category_none),
				rttr::value("tech_category_technology", tech_category::tech_category_technology),
				rttr::value("tech_category_culture", tech_category::tech_category_culture),
				rttr::value("tech_category_magick", tech_category::tech_category_magick)
			);
	}

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(itech)
	{
		rttr::registration::class_<itech>("itech")
			.property("m_name", &itech::m_name)
			.property("m_requirements", &itech::m_requirements)
			.property("m_parent", &itech::m_parent)
			.property("m_race_requirement", &itech::m_race_requirement)
			.property("m_points", &itech::m_points)
			.property("m_category", &itech::m_category)
			;
	};

} //- technology

namespace professions
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(scitizen)
	{
		rttr::registration::class_<scitizen>("scitizen");
	};

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(swoodcutter)
	{
		rttr::registration::class_<swoodcutter>("swoodcutter");
	};

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(sspearman)
	{
		rttr::registration::class_<sspearman>("sspearman");
	};

} //- professions