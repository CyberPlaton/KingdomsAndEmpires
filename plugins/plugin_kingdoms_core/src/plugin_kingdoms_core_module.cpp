#include "plugin_kingdoms_core_module.hpp"

namespace technology
{
	//------------------------------------------------------------------------------------------------------------------------
	bool available_for_race(const ref_t<itech>& tech, kingdoms::kingdom_race race)
	{
		return algorithm::bit_on(tech->m_race_requirement, race);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void itech::save(TArchiveOut& archive) const
	{
		archive(m_name, m_race_requirement, m_points, m_category);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void itech::load(TArchiveIn& archive)
	{
		archive(m_name, m_race_requirement, m_points, m_category);
	}

} //- technology