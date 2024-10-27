#pragma once
#include "../plugin_module_example_common.hpp"
#include "../components/example_components.hpp"

namespace module_example
{
	void targeting_system(flecs::entity e, stargeting_component& target);
	void targeting_system_2(flecs::entity e, stargeting_component& target);
	void targeting_system_3(flecs::entity e, stargeting_component& target);
	void transform_system(flecs::entity e, stransform_component& transform);
	void replication_system(flecs::entity e, sreplicable_component& rep, const stransform_component& trans, const sidentifier_component& id);

	//- Example module system
	//------------------------------------------------------------------------------------------------------------------------
	struct EXAMPLE_API stargeting_system final
	{
		stargeting_system() = default;
		stargeting_system(ecs::cworld* w);

		RTTR_ENABLE();
	};

	//- Duplicate of above system to be used as test in multithreaded environment
	//------------------------------------------------------------------------------------------------------------------------
	struct EXAMPLE_API stargeting_system_2 final
	{
		stargeting_system_2() = default;
		stargeting_system_2(ecs::cworld* w);

		RTTR_ENABLE();
	};

	//- Duplicate of above system to be used as test in multithreaded environment
	//------------------------------------------------------------------------------------------------------------------------
	struct EXAMPLE_API stargeting_system_3 final
	{
		stargeting_system_3() = default;
		stargeting_system_3(ecs::cworld* w);

		RTTR_ENABLE();
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct EXAMPLE_API stransform_system final
	{
		stransform_system() = default;
		stransform_system(ecs::cworld* w);

		RTTR_ENABLE();
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct EXAMPLE_API sreplication_system final
	{
		sreplication_system() = default;
		sreplication_system(ecs::cworld* w);

		RTTR_ENABLE();
	};

} //- module_example