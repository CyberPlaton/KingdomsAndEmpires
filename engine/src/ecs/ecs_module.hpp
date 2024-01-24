#pragma once
#include <flecs.h>
#include <core.h>
#include "ecs_system.hpp"

namespace ecs
{
	namespace detail
	{
		//------------------------------------------------------------------------------------------------------------------------
		struct smodule_info
		{
			stringview_t m_name;
			vector_t<string_t> m_module_dependencies;
			vector_t<string_t> m_module_systems;
		};

		//------------------------------------------------------------------------------------------------------------------------
		class cmodule_database final
		{
		public:
			STATIC_INSTANCE(cmodule_database, s_cmodule_database);

			void module_add(smodule_info& info);

		private:
			umap_t<unsigned, smodule_info> m_modules;
		};

	} //- detail





	//- Current implementation directly registers the module with all components and systems.
	//- Registering a module to load should probably be defined elsewhere.
	//- Thus sinfo is misplaced here too.
	//------------------------------------------------------------------------------------------------------------------------
	template<class TModuleType>
	class imodule
	{
	public:
		imodule(flecs::world& w) :
			m_world(w)
		{
			auto type = rttr::type::get<TModuleType>();

			m_info.m_name = type.get_name().data();
		}
		virtual ~imodule() = default;

		inline stringview_t name() const { return m_info.m_name.data(); }
		imodule& begin_module()
		{
			world().module<TModuleType>(name());
		}
		void end_module()
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

	protected:
		inline const flecs::world& world() const { return m_world; }
		inline flecs::world& world() { return m_world; }
		inline const sinfo& info() const { return m_info; }

	private:
		flecs::world& m_world;
		detail::smodule_info m_info;
	};

} //- ecs