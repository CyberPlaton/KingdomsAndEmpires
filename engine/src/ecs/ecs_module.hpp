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
	template<class TModuleType>
	class imodule
	{
		friend class detail::cmodule_database;

	public:
		imodule(flecs::world& w) :
			m_world(&w)
		{
		}

		virtual ~imodule() = default;

		imodule& begin()
		{
			world().module<TModuleType>();
			return *this;
		}

		void end()
		{
			//- no-op
		}

		template<class TDependency>
		imodule& depends_on()
		{
			world().import<TDependency>();

// 			auto type = rttr::type::get<TDependency>();
// 
// 			if (type.is_valid())
// 			{
// 				const auto* name = type.get_name().data();
// 
// 				//- ignore duplicate entries
// 				if (!algorithm::find(m_info.m_module_dependencies.begin(), m_info.m_module_dependencies.end(), name))
// 				{
// 					m_info.m_module_dependencies.emplace_back(name);
// 				}
// 			}
			return *this;
		}

		template<class TSystem>
		imodule& subsystem()
		{
			static_assert(std::is_base_of<csystem, TSystem>::value, "Subsystems must be derived from csystem class");

			TSystem(world());

// 			auto type = rttr::type::get<TSystem>();
// 
// 			if (type.is_valid())
// 			{
// 				const auto* name = type.get_name().data();
// 
// 				//- ignore duplicate entries
// 				if (!algorithm::find(m_info.m_module_systems.begin(), m_info.m_module_systems.end(), name))
// 				{
// 					m_info.m_module_systems.emplace_back(name);
// 				}
// 			}
			return *this;
		}

		template<class TComponent>
		imodule& component()
		{
			//- TODO: Verify that the component is registered in the correct scope
			//- of the module or whether he has to be deferred too.
			//- This however will prove difficult like this and we may have to rely on capturing
			//- functions instead.
			world().component<TComponent>();
			return *this;
		}

	private:
		flecs::world* m_world;

	private:
		flecs::world& world() {ASSERT(m_world, "World for module was not set!"); return *m_world; }
	};

	namespace detail
	{
		//- database containing all currently registered modules
		//------------------------------------------------------------------------------------------------------------------------
		class cmodule_database
		{
		public:
			STATIC_INSTANCE(cmodule_database, s_cmodule_database);

			template<class TModuleType>
			void push()
			{
				static_assert(std::is_base_of<imodule<TModuleType>, TModuleType>::value, "Modules must be derived from imodule class");

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