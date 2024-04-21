#pragma once
#include <core.h>
#include <engine.h>

namespace kingdoms
{
	constexpr rttr::string_view C_DISPLAY_NAME_PROP = "DisplayName";

	using kingdom_t = handle_type_t;
	using technology_t = handle_type_t;
	using unit_level_t = handle_type_t;
	using player_id_t = handle_type_t;

	//- Each player of current session has this central component.
	//------------------------------------------------------------------------------------------------------------------------
	struct splayer : ecs::icomponent
	{
		DECLARE_COMPONENT(splayer);

		player_id_t m_id;

		RTTR_ENABLE(ecs::icomponent);
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum building_slot_type : uint8_t
	{
		building_slot_type_none = 0,
		building_slot_type_normal,
		building_slot_type_port,

	};

	//------------------------------------------------------------------------------------------------------------------------
	enum birthsign_type : uint8_t
	{
		birthsign_type_none			= 0,
		birthsign_type_the_warrior	= 1,
		birthsign_type_the_mage,
		birthsign_type_the_thief,
		birthsign_type_the_serpent,
		birthsign_type_the_lady,
		birthsign_type_the_stallion,
		birthsign_type_the_lord,
		birthsign_type_the_apprentice,
		birthsign_type_the_storm,
		birthsign_type_the_blizzard,
		birthsign_type_the_ritual,
		birthsign_type_the_tower,
		birthsign_type_the_shadow	= 13,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum biome_type : uint8_t
	{
		biome_type_none = 0,
		biome_type_temperate,
		biome_type_tundra,
		biome_type_jungle,
		biome_type_savannah,
		biome_type_sand,
		biome_type_desert,
		biome_type_snow,
		biome_type_ice,
		biome_type_shallow_water,
		biome_type_deep_water,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum settlement_type : uint8_t
	{
		settlement_type_none = BIT(0),
		settlement_type_city = BIT(1),
		settlement_type_fort = BIT(2),
		settlement_type_colony = BIT(3),

		settlement_type_all = settlement_type_city | settlement_type_fort | settlement_type_colony
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum kingdom_race : uint16_t
	{
		kingdom_race_none		= BIT(0),
		kingdom_race_human		= BIT(1),
		kingdom_race_orc		= BIT(2),
		kingdom_race_goblin		= BIT(3),
		kingdom_race_elf		= BIT(4),
		kingdom_race_gnome		= BIT(5),
		kingdom_race_dwarf		= BIT(6),
		kingdom_race_high_troll = BIT(7),
		kingdom_race_dark_elf	= BIT(8),

		kingdom_race_all = kingdom_race_human | kingdom_race_orc | kingdom_race_goblin | kingdom_race_dark_elf |
							kingdom_race_elf | kingdom_race_gnome | kingdom_race_dwarf | kingdom_race_high_troll
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sattributes : ecs::icomponent
	{
		static constexpr unsigned C_ATTRIBUTE_VALUE_MAX = 100;
		static constexpr unsigned C_MAJOR_DEVIATION_NEG = 10;
		static constexpr unsigned C_MAJOR_DEVIATION_POS = 10;
		static constexpr unsigned C_MINOR_DEVIATION_NEG = 5;
		static constexpr unsigned C_MINOR_DEVIATION_POS = 5;

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
		unsigned m_action_points = 0;

		RTTR_ENABLE(ecs::icomponent);
	};


	//------------------------------------------------------------------------------------------------------------------------
	struct sskills : ecs::icomponent
	{
		static constexpr unsigned C_SKILL_VALUE_MAX = 100;
		static constexpr unsigned C_MAJOR_DEVIATION_NEG = 10;
		static constexpr unsigned C_MAJOR_DEVIATION_POS = 10;
		static constexpr unsigned C_MINOR_DEVIATION_NEG = 5;
		static constexpr unsigned C_MINOR_DEVIATION_POS = 5;

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

	//------------------------------------------------------------------------------------------------------------------------
	struct irace : ecs::icomponent
	{
		DECLARE_COMPONENT(irace);

		sattributes m_attributes;
		sskills m_skills;
		sderived_attributes m_derived_attributes;

		RTTR_ENABLE(ecs::icomponent);
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct iunit : ecs::icomponent
	{
		DECLARE_COMPONENT(irace);

		kingdom_race m_race = kingdom_race_none;
		birthsign_type m_birthsign = birthsign_type_none;

		RTTR_ENABLE(ecs::icomponent);
	};

} //- kingdoms

namespace resource
{
	//- base class for resource definitions. A resource should basically be an inherited child with no data
	//- apart from meta data for rttr.
	//------------------------------------------------------------------------------------------------------------------------
	struct iresource : ecs::icomponent
	{
		DECLARE_COMPONENT(iresource);

		RTTR_ENABLE(ecs::icomponent);
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct swood : iresource
	{
		DECLARE_COMPONENT(swood);

		RTTR_ENABLE(iresource);
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sclay : iresource
	{
		DECLARE_COMPONENT(sclay);

		RTTR_ENABLE(iresource);
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sfood : iresource
	{
		DECLARE_COMPONENT(sfood);

		RTTR_ENABLE(iresource);
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sleather : iresource
	{
		DECLARE_COMPONENT(sleather);

		RTTR_ENABLE(iresource);
	};

} //- resource

namespace building
{
	namespace tag
	{
		//- Tags building entity as eligible for building inside a City.
		//------------------------------------------------------------------------------------------------------------------------
		DECLARE_TAG(scity_buildable);

		//- Tags building entity as eligible for building inside a Castle.
		//------------------------------------------------------------------------------------------------------------------------
		DECLARE_TAG(scastle_buildable);

		//- Tags building entity as eligible for building inside a Colony.
		//------------------------------------------------------------------------------------------------------------------------
		DECLARE_TAG(scolony_buildable);

		//- Tags building entity as eligible for building by a specific race.
		//------------------------------------------------------------------------------------------------------------------------
		DECLARE_TAG(shuman_buildable);

		//------------------------------------------------------------------------------------------------------------------------
		DECLARE_TAG(sorc_buildable);

		//------------------------------------------------------------------------------------------------------------------------
		DECLARE_TAG(sgoblin_buildable);

		//------------------------------------------------------------------------------------------------------------------------
		DECLARE_TAG(self_buildable);

		//------------------------------------------------------------------------------------------------------------------------
		DECLARE_TAG(sgnome_buildable);

		//------------------------------------------------------------------------------------------------------------------------
		DECLARE_TAG(sdwarf_buildable);

		//------------------------------------------------------------------------------------------------------------------------
		DECLARE_TAG(shigh_troll_buildable);

		//------------------------------------------------------------------------------------------------------------------------
		DECLARE_TAG(sdark_elf_buildable);

	} //- tag

	//- Component for building definitions
	//------------------------------------------------------------------------------------------------------------------------
	struct sbuilding final : ecs::icomponent
	{
		DECLARE_COMPONENT(sbuilding);

		struct sconstruction_requirements
		{
			using resource_pair_t = core::spair<std::string, unsigned>;
			using technologies_t = vector_t<std::string>;

			vector_t<resource_pair_t> m_cost;
			//- Note: must match the tags of the building entity/prefab.
			int m_races = kingdoms::kingdom_race_none;
			//- Note: must match the tags of the building entity/prefab.
			int m_settlement_type = kingdoms::settlement_type_none;

			std::string m_previous_building;

			kingdoms::building_slot_type m_slot = kingdoms::building_slot_type_none;
			technologies_t m_technologies;

			RTTR_ENABLE();
		};

		struct sdata
		{
			using profession_level_pair_t = core::spair<std::string, unsigned>;

			vector_t<profession_level_pair_t> m_profession_level_requirements;

			RTTR_ENABLE();
		};


		RTTR_ENABLE(ecs::icomponent);
	};
// 	struct ibuilding : ecs::icomponent
// 	{
// 		DECLARE_COMPONENT(ibuilding);
// 
// 		struct srequirements
// 		{
// 			//- pairing { "swood" : 4 }, where swood is a struct and 4 is required amount to build this structure
// 			umap_t<rttr::type, unsigned> m_cost;
// 			//- races that can build this
// 			int m_race = kingdoms::kingdom_race_all;
// 			unsigned m_settlement_citizen_count = 0;
// 			int m_settlement_type = kingdoms::settlement_type_all;
// 			//- required building to build on top of, where the previous one is replaced with this one,
// 			//- if this is left null then building has to be built on empty space
// 			rttr::type m_previous_building;
// 			//- required slot type. Can be only one
// 			kingdoms::building_slot_type m_building_slot;
// 			//- set of technologies that have to be researched for this building
// 			vector_t<rttr::type> m_technologies;
// 		};
// 
// 		struct sdata
// 		{
// 			//- pairing { "swood" : 2 }, where swood is a struct and 2 is amount of consumed resource
// 			using consumption_pair_t = core::spair<rttr::type, unsigned>;
// 			//- pairing { 1 : [{ "swood" : 2 }, ...] }, where 1 is produced amount and
// 			//- second is vector of required resources along with amount of consumption
// 			using production_pair_t = core::spair<unsigned, vector_t<consumption_pair_t>>;
// 			//- pairing { "sbronze" : ... }, where sbronze is a struct and the rest is the production pair from above
// 			using production_map_t = umap_t<rttr::type, production_pair_t>;
// 
// 			std::string m_name;
// 			production_map_t m_production_consumption;
// 			umap_t<rttr::type, unsigned> m_profession_level_requirements;
// 		};
// 
// 		sdata m_data;
// 		srequirements m_requirements;
// 
// 		RTTR_ENABLE(ecs::icomponent);
// 	};

} //- building

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
		std::string m_name;
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
	namespace archetype
	{
		struct sworker{};
		struct smilitary{};

	} //- archetype

	//- both functions have to be defined for a inheriting profession. Input are the resources and amount that is consumed
	//- when producing and output is what is produced, in terms of per tick/turn.
	//------------------------------------------------------------------------------------------------------------------------
	struct sprofession
	{
		static constexpr std::string_view C_RESOURCE_INPUT_FUNC_NAME = "input_resources";
		static constexpr std::string_view C_RESOURCE_OUTPUT_FUNC_NAME = "output_resources";

		using input_resources_func_t = std::function<const umap_t<std::string, unsigned>&()>;
		using output_resources_func_t = std::function<const umap_t<std::string, unsigned>&()>;

		RTTR_ENABLE();
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct swoodcutter : sprofession
	{
		RTTR_ENABLE(sprofession);
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sspearman : sprofession
	{
		RTTR_ENABLE(sprofession);
	};

	//- each kingdom unit starts life as a citizen, when choosing a profession the citizen component
	//- is replaced with the other component and the unit becomes i.e. a 'Spearman' and 'gains' new abilities
	//------------------------------------------------------------------------------------------------------------------------
	struct scitizen : ecs::icomponent
	{
		DECLARE_COMPONENT(scitizen);

		std::string m_profession;

		core::cuuid m_in_settlement;
		core::cuuid m_working_building;
		core::cuuid m_working_resource;

		RTTR_ENABLE(ecs::icomponent);
	};

} //- professions

namespace settlement
{
	//- base class for a city or fort.
	//------------------------------------------------------------------------------------------------------------------------
	struct isettlement : ecs::icomponent
	{
		DECLARE_COMPONENT(isettlement);

		struct sdata
		{
			kingdoms::kingdom_race m_race = kingdoms::kingdom_race_none;
			kingdoms::settlement_type m_type = kingdoms::settlement_type_none;

			vector_t<core::cuuid> m_citizens;
			vector_t<core::cuuid> m_buildings;
			umap_t<std::string, unsigned> m_resources;
		};

		sdata m_data;

		RTTR_ENABLE(ecs::icomponent);
	};

	bool can_settlement_produce_resource(const isettlement& city, const umap_t<std::string, unsigned>& input);

	//------------------------------------------------------------------------------------------------------------------------
	class csettlement_update_system : public ecs::csystem<isettlement>
	{
	public:
		csettlement_update_system(flecs::world& w) :
			ecs::csystem<isettlement>(w, "Settlement Update System")
		{
			build([&](flecs::entity e, isettlement& city)
				{
					//- TODO: system should depend on 'Turn End' phase or system, so that its only
					//- executed after a turn ends

					//- TODO: check that the city belongs to player that had his turn

					//- update citizens
					for (const auto& uuid : city.m_data.m_citizens)
					{
						if (auto citizen = w.lookup(uuid.string().c_str()); citizen.is_valid() && citizen.is_alive())
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
		}
	};

} //- settlement

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
	};

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(sderived_attributes)
	{
		rttr::registration::class_<sderived_attributes>("sderived_attributes")
			.constructor<>()
			(
				rttr::policy::ctor::as_object
			)
			.property("m_health", &sderived_attributes::m_health)
			.property("m_mana", &sderived_attributes::m_mana)
			.property("m_action_points", &sderived_attributes::m_action_points)
			;
	};

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(sskills)
	{
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
	};

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(irace)
	{
		rttr::registration::class_<irace>("irace")
			.constructor<>()
			(
				rttr::policy::ctor::as_object
			)
			.property("m_attributes", &irace::m_attributes)
			.property("m_skills", &irace::m_skills)
			.property("m_derived_attributes", &irace::m_derived_attributes)
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

namespace resource
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(iresource)
	{
		rttr::registration::class_<iresource>("iresource");
	}

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(swood)
	{
		rttr::registration::class_<swood>("swood");
	}

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(sclay)
	{
		rttr::registration::class_<sclay>("sclay");
	}

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(sfood)
	{
		rttr::registration::class_<sfood>("sfood");
	}

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(sleather)
	{
		rttr::registration::class_<sleather>("sleather");
	}

} //- resource

namespace building
{
	//------------------------------------------------------------------------------------------------------------------------
// 	REFLECT_INLINE(ibuilding)
// 	{
// 		rttr::registration::class_<ibuilding>("ibuilding")
// 			.property("m_requirements", &ibuilding::m_requirements)
// 			.property("m_data", &ibuilding::m_data)
// 			;
// 	}

} //- building