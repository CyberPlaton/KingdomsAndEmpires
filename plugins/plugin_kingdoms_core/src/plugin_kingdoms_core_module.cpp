#include "plugin_kingdoms_core_module.hpp"

namespace technology
{
	//------------------------------------------------------------------------------------------------------------------------
	bool available_for_race(const ref_t<itech>& tech, kingdoms::kingdom_race race)
	{
		return algorithm::bit_on(tech->m_race_requirement, race);
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

	//------------------------------------------------------------------------------------------------------------------------
	csettlement_update_system::csettlement_update_system(flecs::world& w) :
		ecs::csystem(w)
	{
		subsystem([&](flecs::world& world) -> subsystem_registrator_return_t
			{
				auto cities_update = world.system<isettlement>("Settlement Update System")
					.each([&](flecs::entity e, isettlement& city)
						{
							//- TODO: system should depend on 'Turn End' phase or system, so that its only
							//- executed after a turn ends

							//- TODO: check that the city belongs to player that had his turn


							//- update citizens
							for (const auto& uuid: city.m_data.m_citizens)
							{
								if (auto citizen = world.lookup(uuid.string().c_str()); citizen.is_valid() && citizen.is_alive())
								{
									//- each citizen that is a worker and can produce things
									if (const auto* c = citizen.get<professions::scitizen>();
										c && citizen.has<professions::archetype::sworker>())
									{
										if (const auto& prof = c->m_profession; !prof.empty())
										{
											if (auto prof_type = rttr::type::get_by_name(prof); prof_type.is_valid())
											{
												auto input_method = prof_type.get_method(professions::sprofession::C_RESOURCE_INPUT_FUNC_NAME.data());
												auto output_method = prof_type.get_method(professions::sprofession::C_RESOURCE_OUTPUT_FUNC_NAME.data());

												//- check whether city is able to produce the output resources
												const auto& input = input_method.invoke({}).get_value<const umap_t<std::string, unsigned>>();
												if (can_settlement_produce_resource(city, input))
												{
													const auto& output = output_method.invoke({}).get_value<const umap_t<std::string, unsigned>>();

													//- reduce count of input resources and increase increase count of output resources
													for (auto& inpair : input)
													{
														//- assured to not go 'negative'
														city.m_data.m_resources[inpair.first] -= inpair.second;
													}
													for (auto& outpair : output)
													{
														city.m_data.m_resources[outpair.first] += outpair.second;
													}

													//- TODO: increase experience amount of citizens profession
												}
											}
										}
									}
								}
							}
						});


				return { cities_update, {}};
			});
	}

} //- settlement
