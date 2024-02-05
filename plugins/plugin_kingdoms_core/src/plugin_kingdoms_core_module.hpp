#pragma once
#include <core.h>
#include <engine.h>
#include <rttr.h>

namespace kingdoms
{
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