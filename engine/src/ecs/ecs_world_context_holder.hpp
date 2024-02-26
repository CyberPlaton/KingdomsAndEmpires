#pragma once
#include <core.h>
#include <flecs.h>

namespace ecs
{
	//- Storing information about all modules, including their systems, dependencies, components etc.
	//------------------------------------------------------------------------------------------------------------------------
	class cmodules_info
	{
	public:
		STATIC_INSTANCE(cmodules_info, s_cmodules_info);

		struct scomponent_info
		{
			std::string m_name;
		};

		struct ssystem_info
		{
			std::string m_name;
			vector_t<std::string> m_deps;
		};

		struct smodule_info
		{
			std::string m_name;
			vector_t<smodule_info> m_deps;
			vector_t<ssystem_info> m_systems;
			vector_t<scomponent_info> m_comps;
		};

		smodule_info& module(const std::string& name);

		ssystem_info& system(const std::string& name);

		scomponent_info& component(const std::string& name);

	private:

	};


	//- Note: context holds pointer in order to make the class move constructible,
	//- this is required by flecs.
	//------------------------------------------------------------------------------------------------------------------------
	class iworld_context_holder
	{
	public:
		iworld_context_holder(flecs::world& w);
		~iworld_context_holder();

	private:
		flecs::world* m_world;

	protected:
		flecs::world& world();
		const flecs::world& world() const;

		RTTR_ENABLE();
		RTTR_REFLECTABLE();
	};

} //- ecs

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(iworld_context_holder)
	{
		rttr::registration::class_<iworld_context_holder>("iworld_context_holder")
			.constructor<flecs::world&>()
			(
				rttr::policy::ctor::as_raw_ptr
			)
			;
	}

} //- ecs