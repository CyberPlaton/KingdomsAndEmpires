#pragma once
#include <engine.h>

namespace kingdoms
{
	namespace component
	{
		//------------------------------------------------------------------------------------------------------------------------
		struct sattack final : public ecs::icomponent
		{
			DECLARE_COMPONENT(sattack);

			enum damage_type : uint8_t
			{
				damage_type_none = 0,
				damage_type_slashing,	//- e.g. a sword type weapon etc.
				damage_type_thrusting,	//- e.g. a spear type weapon or an arrow etc.
				damage_type_blunt,		//- e.g. a hammer type weapon or thrown rocks etc.
			};

			enum attack_type : uint8_t
			{
				attack_type_none = 0,
				attack_type_melee,
				attack_type_range,
			};

			unsigned m_damage = 0;
			unsigned m_damage_deviation = 0;
			unsigned m_range_min = 0;
			unsigned m_range_max = 0;
			damage_type m_damage_type = damage_type_none;
			attack_type m_attack_type = attack_type_none;

			RTTR_ENABLE(icomponent);
		};

		//------------------------------------------------------------------------------------------------------------------------
		struct sdefense final : public ecs::icomponent
		{
			DECLARE_COMPONENT(sdefense);

			enum armor_type : uint8_t
			{
				armor_type_none = 0,
				armor_type_unarmored,
				armor_type_light,
				armor_type_medium,
				armor_type_heavy,
			};

			unsigned m_armor_value = 0;
			armor_type m_armor_type = armor_type_none;

			RTTR_ENABLE(icomponent);
		};

		//------------------------------------------------------------------------------------------------------------------------
		struct sstatus_effects final : public ecs::icomponent
		{
			DECLARE_COMPONENT(sstatus_effects);


			RTTR_ENABLE(icomponent);
		};

		//------------------------------------------------------------------------------------------------------------------------
		struct sunit final : public ecs::icomponent
		{
			DECLARE_COMPONENT(sunit);

			enum movement_type : uint8_t
			{
				movement_type_none = 0,
				movement_type_walking,
				movement_type_mounted,
				movement_type_swimming,
				movement_type_flying,
				movement_type_driving,
			};

			unsigned m_action_points = 0;
			unsigned m_movement_points = 0;
			int m_health = 0;
			unsigned m_mana = 0;
			movement_type m_movement_type = movement_type_none;

			RTTR_ENABLE(icomponent);
		};

		//------------------------------------------------------------------------------------------------------------------------
		struct svision final : ecs::icomponent
		{
			DECLARE_COMPONENT(svision);

			unsigned m_distance = 0;

			RTTR_ENABLE(icomponent);
		};

	} //- component

} //- kingdoms