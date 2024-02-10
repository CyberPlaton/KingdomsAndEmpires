#pragma once
#include "ecs_world_context_holder.hpp"

namespace ecs
{
	//- system interface. Note that flecs::system is basically an entity,
	//- we take advantage of this by assigning a name to it.
	//------------------------------------------------------------------------------------------------------------------------
	class csystem : public iworld_context_holder
	{
	public:
		using subsystem_registrator_return_t = std::pair<flecs::system, vector_t<flecs::system>>;
		using subsystem_registrator_t = std::function<subsystem_registrator_return_t(flecs::world&)>;

		csystem(flecs::world& w);
		virtual ~csystem() = default;

		//- argument function is responsible for registering subsystem(s) to the world.
		void subsystem(subsystem_registrator_t function);

		auto self() const {return m_self;}
		auto name() const {return m_self.name().c_str(); }
		vector_t<flecs::system> subsystems() const;

	private:
		flecs::system m_self;
		vector_t<flecs::system> m_subsystems;
	};


} //- ecs