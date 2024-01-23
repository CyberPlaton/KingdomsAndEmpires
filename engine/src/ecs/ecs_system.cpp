#include "ecs_system.hpp"

namespace ecs
{
} //- ecs

namespace ecs::example
{
	struct sbehavior_component
	{
		stringview_t m_script;
	};

	struct sdestructible_component
	{
		enum type
		{
			physagreggator,
			dynamic,
			duplicating,
		};

		stringview_t m_path;
		type m_type;
	};

	struct sphysical_trigger_component
	{
		enum type
		{
			activating,
			deactivating,
		};

		type m_type;
		int m_value;
	};
}