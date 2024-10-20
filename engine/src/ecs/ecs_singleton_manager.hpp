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
		csingleton_manager(cworld* w);

		template<typename TSingleton>
		csingleton_manager& add_singleton();
		csingleton_manager& add_singleton(rttr::type type);
		csingleton_manager& add_singleton(stringview_t type_name);

		const vector_t<std::string>& singletons() const;

	private:
		vector_t<std::string> m_registered_singletons;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TSingleton>
	csingleton_manager& csingleton_manager::add_singleton()
	{
		return add_singleton(rttr::type::get<TSingleton>());
	};

} //- ecs