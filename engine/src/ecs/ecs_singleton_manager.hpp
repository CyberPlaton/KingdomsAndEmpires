#pragma once
#include <core.h>
#include "ecs_world_context_holder.hpp"
#include "ecs_singleton.hpp"

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	class csingleton_manager : public iworld_context_holder
	{
	public:
		csingleton_manager(flecs::world& w);

		template<class TComponent>
		TComponent* add();

		template<class TComponent, typename... ARGS>
		TComponent* set(ARGS&&... args);

		template<class TComponent>
		const TComponent* get() const;

		template<class TComponent>
		TComponent* modify();

		const vector_t<std::string>& singletons() const;

	private:
		vector_t<std::string> m_registered_singletons;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<class TComponent>
	TComponent* csingleton_manager::add()
	{
		world().set<TComponent>();

		return world().get_mut<TComponent>();
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TComponent, typename... ARGS>
	TComponent* csingleton_manager::set(ARGS&&... args)
	{
		world().set<TComponent>({ std::forward<ARGS>(args)... });

		return world().get_mut<TComponent>();
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TComponent>
	const TComponent* csingleton_manager::get() const
	{
		return world().get<TComponent>();
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TComponent>
	TComponent* csingleton_manager::modify()
	{
		return world().get_mut<TComponent>();
	}

} //- ecs