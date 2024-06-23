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
