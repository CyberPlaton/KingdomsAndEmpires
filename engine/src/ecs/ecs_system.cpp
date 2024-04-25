#include "ecs_system.hpp"
#include "ecs_component.hpp"
#include "ecs_world_manager.hpp"
#include "ecs_singleton.hpp"

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	void cfree_system::run_after(flecs::entity e)
	{
		CORE_ASSERT(e.is_valid(), "Invalid operation. Specified system does not exist!");

		m_self.add(flecs::Phase).depends_on(e);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cfree_system::run_after(stringview_t name)
	{
		run_after(world().lookup(name.data()));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cfree_system::run_after(const flecs::entity_t e)
	{
		m_self.add(flecs::Phase).depends_on(e);
	}

} //- ecs

namespace ecs::example
{
	//------------------------------------------------------------------------------------------------------------------------
	struct sbehavior_component : public icomponent
	{
		DECLARE_COMPONENT(sbehavior_component);

		stringview_t m_script;
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sdestructible_component : public icomponent
	{
		DECLARE_COMPONENT(sdestructible_component);

		enum type
		{
			physagreggator,
			dynamic,
			duplicating,
		};

		stringview_t m_path;
		type m_type;
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct sphysical_trigger_component : public icomponent
	{
		DECLARE_COMPONENT(sphysical_trigger_component);

		enum type
		{
			activating,
			deactivating,
		};

		type m_type;
		int m_value;
	};

	//- Example system. Missing is the registration in RTTR, for this checkout plugin_module_example
	//------------------------------------------------------------------------------------------------------------------------
	class csystem_example final : public csystem<sdestructible_component, sphysical_trigger_component>
	{
	public:
		csystem_example(flecs::world& w) :
			csystem<sdestructible_component, sphysical_trigger_component>
			(w, "csystem_example")
		{
			build([&](flecs::entity e, sdestructible_component& destr, const sphysical_trigger_component& trigg)
				{
					//- with this we only define one system
				});

			run_after(flecs::OnUpdate);
		}
	};

} //- ecs::example