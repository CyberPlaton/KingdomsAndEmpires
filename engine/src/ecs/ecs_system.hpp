#pragma once
#include <flecs.h>
#include <core.h>

namespace ecs
{

	template<typename TCallable, class TComponent, class ... ARGS>
	class csystem
	{
	public:
		csystem(flecs::world& world)
		{
			m_system = world.system<TComponent, ARGS...>();
		}

	private:
		flecs::system m_system;
	};




	//- System interface. Should not be used for inheritance.
	//------------------------------------------------------------------------------------------------------------------------
	class isystem
	{
	public:
		isystem(flecs::system system);
		virtual ~isystem() = default;

		virtual bool register_system() = 0;

		inline flecs::system& system() { return m_system; }
		inline const flecs::system& system() const { return m_system; }

	private:
		flecs::system m_system;

		RTTR_ENABLE();
	};


} //- ecs