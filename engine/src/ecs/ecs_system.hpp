#pragma once
#include <flecs.h>
#include <core.h>

namespace ecs
{
	//- System interface.
	//------------------------------------------------------------------------------------------------------------------------
	class csystem
	{
	public:
		using subsystem_registrator_t = std::function<void(flecs::world&)>;

		csystem(flecs::world& w);
		virtual ~csystem() = default;

		//- argument function is responsible for registering subsystem(s)
		//- to the world.
		csystem& subsystem(subsystem_registrator_t function);

	protected:
		inline const flecs::world& world() const { return m_world; }
		inline flecs::world& world() { return m_world; }

	private:
		flecs::world& m_world;
	};


} //- ecs