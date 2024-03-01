#pragma once
#include "ecs_world_context_holder.hpp"
#include "ecs_singleton.hpp"

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	enum system_running_phase
	{
		system_running_phase_none = 0,
		system_running_phase_on_update,
		system_running_phase_on_world_render,
		system_running_phase_on_ui_render,
		system_running_phase_on_post_update, //- Note: fixed dt. After all rendering is done.
	};

	//------------------------------------------------------------------------------------------------------------------------
	class isystem
	{
	public:
		virtual flecs::entity self() const = 0;
		virtual stringview_t name() const = 0;
	};

	//- system interface. Note that flecs::system is basically an entity,
	//- we take advantage of this by assigning a name to it.
	//-
	//- when defining used components do
	//- class cexample : public csystem<ecs::stransform, ecs::sidentifier>
	//-
	//- what really matters for runtime is the 'build' function:
	//- you can define const ecs::stransform& there as well as ecs::stransform&
	//- and those as what will be passed to the function.
	//-
	//- TCallable system function should be as in system_function_prototype_t, i.e.
	//- entity first and then your used components as described above.
	//-
	//- Note: it is possible to force a system to run on more than one phase, i.e.
	//- run_on(system_running_phase_on_update | system_running_phase_on_ui_render);
	//- just make sure you know what you are doing.
	//------------------------------------------------------------------------------------------------------------------------
	template<typename... TComps>
	class csystem : public isystem,
		public iworld_context_holder
	{
	public:
		using system_function_prototype_t = std::function<void(flecs::entity, TComps...)>;

		csystem(flecs::world& world, const std::string& name) :
			iworld_context_holder(world),
			m_builder(world.system<TComps...>(name.c_str()))
		{
		}
		~csystem() = default;

		flecs::entity self() const override final { return m_self; }
		stringview_t name() const override final { return m_self.name().c_str(); }

	protected:
		template<class TSystem>
		void depends_on();

		//- Marks the system as available for threading. Note that a system using ImGui UI
		//- is not eligible for multithreading.
		void multithreaded();

		//- Changes made by this system are visible immediately. Normally,
		//- any structural changes are inserted into a command buffer and synced
		//- before next tick. Enabling this makes the system single threaded.
		//- Note: Structural changes are for example adding or removing components from an entity.
		void immediate();

		void run_on(system_running_phase p);

		void run_after(flecs::entity e);

		void run_after(stringview_t name);

		template<typename TCallable>
		void build(TCallable&& func);

	private:
		flecs::system m_self;
		flecs::system_builder<TComps...> m_builder;
		system_running_phase m_phase;
		bool m_multithreaded = false;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename... TComps>
	template<class TSystem>
	void csystem<TComps...>::depends_on()
	{
		//- Note: registers a system we depend on AFTER we are registered to flecs
		TSystem system(world());
	}

	//- Marks the system as available for threading. Note that a system using ImGui UI
	//- is not eligible for multithreading.
	//------------------------------------------------------------------------------------------------------------------------
	template<typename... TComps>
	void csystem<TComps...>::multithreaded()
	{
		CORE_ASSERT(m_phase != system_running_phase_on_ui_render,
			"Invalid operation. A UI system can not be multithreaded");

		m_builder.multi_threaded();
		m_multithreaded = true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename... TComps>
	void csystem<TComps...>::immediate()
	{
		m_builder.no_readonly();
		m_multithreaded = false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename... TComps>
	void csystem<TComps...>::run_on(system_running_phase p)
	{
		const auto* phases = world().template get<ssystem_phases>();

		switch (p)
		{
		case system_running_phase_on_update:
		{
			m_builder.kind(phases->m_phases[ssystem_phases::C_ON_UPDATE])
				.kind<ssystem_phases::son_update>();
			break;
		}
		case system_running_phase_on_world_render:
		{
			m_builder.kind(phases->m_phases[ssystem_phases::C_ON_WORLD_RENDER])
				.kind<ssystem_phases::son_world_render>();
			break;
		}
		case system_running_phase_on_ui_render:
		{
			CORE_ASSERT(!m_multithreaded, "Invalid operation. A UI system can not be multithreaded");

			m_builder.kind(phases->m_phases[ssystem_phases::C_ON_UI_RENDER])
				.kind<ssystem_phases::son_ui_render>();
			break;
		}
		case system_running_phase_on_post_update:
		{
			m_builder.kind(phases->m_phases[ssystem_phases::C_ON_POST_UPDATE])
				.kind<ssystem_phases::son_post_update>();
			break;
		}
		default:
		case system_running_phase_none:
		{
			CORE_ASSERT(false, "Invalid operation. Phase is not valid");
			break;
		}
		}
		m_phase = p;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename... TComps>
	void csystem<TComps...>::run_after(flecs::entity e)
	{
		m_builder.kind(e);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename... TComps>
	void csystem<TComps...>::run_after(stringview_t name)
	{
		run_after(world().lookup(name));
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename... TComps>
	template<typename TCallable>
	void csystem<TComps...>::build(TCallable&& func)
	{
		m_self = m_builder.each(func);
	}

	//- A free system is similar to a normal system, only that it does not match any components and thus no entities.
	//- If entities are required they can be retrieved through the world or a query.
	//------------------------------------------------------------------------------------------------------------------------
	class cfree_system : public isystem,
		public iworld_context_holder
	{
	public:
		using system_function_prototype_t = std::function<void(flecs::iter&)>;

		cfree_system(flecs::world& world, const std::string& name) :
			iworld_context_holder(world),
			m_builder(world.system(name.c_str()))
		{
		}
		~cfree_system() = default;

		flecs::entity self() const override final { return m_self; }
		stringview_t name() const override final { return m_self.name().c_str(); }

		template<class TSystem>
		void depends_on();

		void run_on(system_running_phase p);

		void run_after(flecs::entity e);

		void run_after(stringview_t name);

		template<typename TCallable>
		void build(TCallable&& callback);

	private:
		flecs::system m_self;
		system_running_phase m_phase;
		flecs::system_builder<> m_builder;
	};


	//------------------------------------------------------------------------------------------------------------------------
	template<typename TCallable>
	void cfree_system::build(TCallable&& callback)
	{
		m_self = m_builder.iter(callback);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TSystem>
	void cfree_system::depends_on()
	{
		//- Note: registers a system we depend on AFTER we are registered to flecs
		TSystem system(world());
	}

} //- ecs