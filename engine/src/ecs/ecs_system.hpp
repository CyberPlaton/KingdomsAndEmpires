#pragma once
#include "ecs_world_context_holder.hpp"

namespace ecs
{
	//- System interface.
	//------------------------------------------------------------------------------------------------------------------------
	class ENGINE_API csystem : public iworld_context_holder
	{
	public:
		using subsystem_registrator_return_t = std::pair<flecs::system, vector_t<flecs::system>>;
		using subsystem_registrator_t = std::function<subsystem_registrator_return_t(flecs::world&)>;

		csystem(ref_t<flecs::world> w);
		virtual ~csystem() = default;

		flecs::system system() const;
		vector_t<flecs::system> subsystems() const;

		//- argument function is responsible for registering subsystem(s) to the world.
		void subsystem(subsystem_registrator_t function);

	private:
		flecs::system m_system;
		vector_t<flecs::system> m_subsystems;
	};


} //- ecs