#pragma once
#include <flecs.h>
#include <core.h>
#include "ecs_system.hpp"

namespace ecs
{
	namespace detail
	{
		//- forward decl.
		class cmodule_database;
	}

	//- utility class simplifying registering a module into ecs
	//------------------------------------------------------------------------------------ ------------------------------------
	class imodule
	{
	public:
		imodule(flecs::world& w) : m_world(&w) {}

		template<class TModuleType>
		imodule* begin()
		{
			world().module<TModuleType>();

			return this;
		}

		template<class TDependency>
		imodule* depends_on()
		{
			world().import<TDependency>();

			return this;
		}

		template<class TSystem>
		imodule* subsystem()
		{
			TSystem(world());

			return this;
		}

		template<class TComponent>
		imodule* comp()
		{
			world().component<TComponent>();

			return this;
		}

		bool end()
		{
			return true;
		}

	private:
		flecs::world* m_world;

	private:
		flecs::world& world() { ASSERT(m_world, "World for module was not set!"); return *m_world; }

		RTTR_ENABLE();
	};

	namespace detail
	{
		//- database containing all currently registered modules.
		//- The modules can be retrieved from RTTR with rttr::type::get_by_name("my_module");
		//------------------------------------------------------------------------------------------------------------------------
		class cmodule_database
		{
		public:
			STATIC_INSTANCE(cmodule_database, s_cmodule_database);

			template<class TModuleType>
			void push()
			{
				m_modules.insert(rttr::type::get<TModuleType>().get_name().data());
			}

			const uset_t<stringview_t>& modules() const;

		private:
			uset_t<stringview_t> m_modules;
		};

	} //- detail

} //- ecs

#define ECS_MODULE(c) \
ecs::detail::cmodule_database::instance().push<c>()