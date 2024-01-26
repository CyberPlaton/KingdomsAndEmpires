#pragma once
#include <config.hpp>
#include <flecs.h>
#include <core.h>

namespace ecs
{
	//- System interface.
	//------------------------------------------------------------------------------------------------------------------------
	class ENGINE_API csystem
	{
	public:
		using subsystem_registrator_t = std::function<void(flecs::world&)>;

		csystem(flecs::world& w);
		virtual ~csystem() = default;

		//- argument function is responsible for registering subsystem(s) to the world.
		csystem& subsystem(subsystem_registrator_t function);

	private:
		flecs::world& m_world;

	private:
		inline const flecs::world& world() const { return m_world; }
		inline flecs::world& world() { return m_world; }

		RTTR_ENABLE();
	};


} //- ecs