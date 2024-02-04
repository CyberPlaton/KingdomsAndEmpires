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
			value("kingdom_race_troll",		kingdom_race::kingdom_race_troll),
			value("kingdom_race_dark_elf",	kingdom_race::kingdom_race_dark_elf))
		;

	registration::class_<itech>("itech")
		.property("m_name", &itech::m_name)
		.property("m_requirements", &itech::m_requirements)
		.property("m_parent", &itech::m_parent)
		.property("m_race_requirement", &itech::m_race_requirement)
		.property("m_points", &itech::m_points)
		.property("m_category", &itech::m_category)
		.property("m_callback", &itech::m_callback)
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