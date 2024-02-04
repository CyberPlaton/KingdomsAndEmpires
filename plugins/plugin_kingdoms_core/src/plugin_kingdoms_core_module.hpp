#pragma once
#include <core.h>
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
		kingdom_race_troll,
		kingdom_race_dark_elf,
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